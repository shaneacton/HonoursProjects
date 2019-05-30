import math
import matplotlib.pyplot as plt

nodeCrossSection = 1
maxThreads = 8
maxSize = 12500000
methodNames = ["openmp", "regular openmp", "MPI", "MPI Regular"]

values = {"sequential":{"sizeVtime":([],[])},
                                "openmp" : {"threadsVspeedup":([],[]), "sizeVspeedup":([],[]), "nodesVspeedup":([],[])},
                                "regular openmp" :{"threadsVspeedup":([],[]), "sizeVspeedup":([],[]), "nodesVspeedup":([],[])},
                                "MPI":{"threadsVspeedup":([],[]), "sizeVspeedup":([],[]), "nodesVspeedup":([],[])},
                                "MPI Regular" : {"threadsVspeedup":([],[]), "sizeVspeedup":([],[]), "nodesVspeedup":([],[])}}

#{"openmp" : {"threadsVspeedup":([threads],[speedup]), "sizeVspeedup":([sizes],[speedup])}}

def addDataPoint(methodName, threads, size, nodes, time):
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

        if(threads == maxThreads and nodes == nodeCrossSection):
            sizes,speedUps = values[methodName]["sizeVspeedup"]
            sizes.append(size)
            speedUps.append(speedUp)
        if(size == maxSize and nodes == nodeCrossSection):
            threadsArr, speedUps = values[methodName]["threadsVspeedup"]
            threadsArr.append(threads)
            speedUps.append(speedUp)

        if (size == maxSize and threads == maxThreads ):
            nodesArr, speedUps = values[methodName]["nodesVspeedup"]
            nodesArr.append(nodes)
            speedUps.append(speedUp)


    pass

def readResults(nodeNum):
    f = open('results'+str(nodeNum)+'.txt','r')

    methodName = ""
    threads = 0
    size = 0
    nodes = 0

    sum =0
    count =0

    for l in f:
        if(l.__contains__("experiment")):
            addDataPoint(methodName, threads, size,nodes, (sum / count))
            break

        line = l.replace("\n","")
        if(len(line)==0):
            continue
        #print (line)

        if(line[0] == '*'):
            if(len(methodName)>0 and sum > 0):
                addDataPoint(methodName,threads,size,nodes,(sum/count))
                sum =0
                count =0
            methodName = line[1:]
            #print (methodName)
            continue

        if(line[0] == '#'):
            if (len(methodName) > 0):
                addDataPoint(methodName, threads, size,nodes, (sum / count))
                sum = 0
                count = 0
            print(line)
            threads = int(line.split(',')[1])
            size = int(line[1:].split(',')[0])
            nodes = int(line.split(',')[2])
            #
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
        plt.legend(loc=(0.98, 0.5))

    if('size' in type):
        plt.xscale('log')
    plt.xlabel(type)
    plt.ylabel("speed up")
    plt.show()

for i in range(1,4):
    readResults(i)

#plotResults("sequential")
#plotResults("threads")
#plotResults("size")
plotResults("nodes")




print(values["regular openmp"]["threadsVspeedup"])