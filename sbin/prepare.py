#!/usr/bin/python
from sets import Set


def masterXml(xmlfl, maddr, wlist):
    f = open(xmlfl, 'w+')
    wlstr = ""
    for w in wlist:
        wlstr = wlstr + "\t\t<worker>{worker}</worker>\n".format(worker=w)

    body = """
<configure>
    <master>{master}</master>
    <workers>
{workerlist}
    </workers>
</configure>
    """.format(master=maddr, workerlist=wlstr)
    print >> f, body
    f.close()


def workerXml(xmlfl, haddr, maddr, startid, endid, dbpath, graphpath, wlist):
    f = open(xmlfl, 'w+')
    wlstr = ""
    for w in wlist:
        wlstr = wlstr + "\t\t<worker>{worker}</worker>\n".format(worker=w)

    body = """
<configure>
    <host>{host}</host>
    <master>{master}</master>
    <nodestart>{nstart}</nodestart>
    <nodeend>{nend}</nend>
    <dbpath>{dbpath}</dbpath>
    <graphpath>{graphpath}</graphpath>
    <workers>
{workerlist}
    </workers>
</configure>
    """.format(
        host=haddr,
        master=maddr,
        nstart=startid,
        nend=endid,
        dbpath=dbpath,
        graphpath=graphpath,
        workerlist=wlstr
        )
    print >> f, body
    f.close()


def splitGraph(graphfl, workerN):
    f = open(graphfl, 'r')
    flst = list()
    for i in range(workerN):
        flst.append(open('%s-%d' % (graphfl, i), 'w+'))
    nodeset = Set()
    lines = f.readlines()
    for line in lines:
        edgestr = line.split()
        nodeu = int(edgestr[0])
        nodev = int(edgestr[1])
        nodeset.add(nodeu)
        nodeset.add(nodev)
    nodes = list(nodeset)
    nodes.sort()
    splits = list()
    nodeN = len(nodes)
    nodes.append(nodes[nodeN - 1] + 1)
    for i in range(workerN):
        splits.append((
            nodes[i * nodeN / workerN],
            nodes[(i + 1) * nodeN / workerN]))

    for line in lines:
        edgestr = line.split()
        nodeu = int(edgestr[0])
        nodev = int(edgestr[1])
        mu = 0
        mv = 0
        for i in range(workerN):
            if nodeu < splits[i][1]:
                mu = i
                break
        for i in range(workerN):
            if nodev < splits[i][1]:
                mv = i
                break
        flst[mu].write(line)
        if not mu == mv:
            flst[mu].write(line)

    f.close()
    for subf in flst:
        subf.close()
    return splits


if __name__ == '__main__':
    machineip = "127.0.0.1"
    mport = 50051
    wport_base = 50060
    workerN = 10
    rootpath = '/home/leckie/Tregel'
    graphfl = rootpath + '/data/web-Stanford.txt'
    xmlm = rootpath + '/conf/master.xml'
    xmlw = rootpath + '/conf/worker{i}.xml'
    dbfl = rootpath + '/tmp/'

    maddr = "%s:%d" % (machineip, mport)
    splits = splitGraph(graphfl, workerN)

    wlist = []
    for i in range(workerN):
        wlist.append("%s:%d" % (machineip, wport_base + i))

    masterXml(xmlm, maddr, wlist)
    for i in range(workerN):
        xmlwi = xmlw.format(i=i)
        waddr = "%s:%d" % (machineip, wport_base + i)
        subgrph = "%s-%d" % (graphfl, i)
        workerXml(
            xmlwi,
            waddr,
            maddr,
            splits[i][0],
            splits[i][1],
            dbfl + 'db' + str(i),
            subgrph,
            wlist)
