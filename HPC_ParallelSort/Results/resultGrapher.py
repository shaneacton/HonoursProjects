import math
import matplotlib.pyplot as plt

nodeCrossSection = 1
maxThreads = 8
maxSize = 12500000
methodNames = ["openmp", "regular openmp", "MPI", "MPI Regular"]

values = {"sequential":{"sizeVtime":([],[])},
                                "openmp" : {"threadsVspeedup":([],[]), "sizeVspeedup":([],[])},
                                "regular openmp" :{"threadsVspeedup":([],[]), "sizeVspeedup":([],[])},
                                "MPI":{"threadsVspeedup":([],[]), "sizeVspeedup":([],[])},
                                "MPI Regular" : {"threadsVspeedup":([],[]), "sizeVspeedup":([],[])}}

#{"openmp" : {"threadsVspeedup":([threads],[speedup]), "sizeVspeedup":([sizes],[speedup])}}

def addDataPoint(methodName, threads, size, time):
    print("adding data point:",methodName,threads,size,time)
    if("seq" in methodName):
        sizes, times = values[methodName]["sizeVtime"]
        sizes.append(size)
        times.append(time)
    else:
        sizes,seqTimes = values["sequential"]["sizeVtime"]
        index = sizes.index(size)
        seqTime = seqTimes[index]
        speedUp = seqTime/time

        if(threads == maxThreads):
            sizes,speedUps = values[methodName]["sizeVspeedup"]
            sizes.append(size)
            speedUps.append(speedUp)
        if(size == maxSize):
            threadsArr, speedUps = values[methodName]["threadsVspeedup"]
            threadsArr.append(threads)
            speedUps.append(speedUp)
    pass

def readResults():
    f = open('results'+nodeCrossSection+'.txt','r')

    methodName = ""
    threads = 0
    size = 0

    sum =0
    count =0

    for l in f:
        if(l.__contains__("experiment")):
            addDataPoint(methodName, threads, size, (sum / count))
            sum = 0
            count = 0
            break

        line = l.replace("\n","")
        if(len(line)==0):
            continue
        #print (line)

        if(line[0] == '*'):
            if(len(methodName)>0 and sum > 0):
                addDataPoint(methodName,threads,size,(sum/count))
                sum =0
                count =0
            methodName = line[1:]
            #print (methodName)
            continue

        if(line[0] == '#'):
            if (len(methodName) > 0):
                addDataPoint(methodName, threads, size, (sum / count))
                sum = 0
                count = 0

            threads = int(line.split(',')[1])
            size = int(line[1:].split(',')[0])
            #print(line)
            continue

        ##########value########

        sum += float(line)
        count +=1

def plotResults(type):
    if("seq" in type):
        vals, times = values["sequential"]["sizeVtime"]
        plt.plot(vals,times,"go-")
        plt.xscale('log')
        plt.xlabel(type)
        plt.ylabel("time (seconds)")

        plt.show()
        return
    for method in methodNames:
        colour = "r" if "MPI" in method else "b"
        shape = "^-" if "egula" in method else "o-"

        vals, speedUps = values[method][type + "Vspeedup"]
        plt.plot(vals,speedUps, (colour + shape), label = method)
        plt.legend(loc='upper left')

    if('size' in type):
        plt.xscale('log')
    plt.xlabel(type)
    plt.ylabel("speed up")
    plt.show()

readResults()
plotResults("sequential")
plotResults("threads")
plotResults("size")




print(values["regular openmp"]["threadsVspeedup"])