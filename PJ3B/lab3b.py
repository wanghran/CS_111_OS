import sys


def Audit(summary):

    blockMap = {}
    inodeMap = {}
    global first_nonreserved_inode
    global first_nonreserved_block
    global block_count
    global inode_count

    for line in summary:
        splitLine = line.split(',')

        if splitLine[0] == 'SUPERBLOCK':
            block_size = int(splitLine[3])
            inode_size = int(splitLine[4])
            first_nonreserved_inode = int(splitLine[7].split('\n')[0])

        if splitLine[0] == 'GROUP':
            block_count = int(splitLine[2])
            inode_count = int(splitLine[3])
            inodeTable = int(splitLine[8].split('\n')[0])
            first_nonreserved_block = int(inodeTable + inode_count * inode_size / block_size)

        if splitLine[0] == 'BFREE':
            block_num = int(splitLine[1].split('\n')[0])
            blockMap[block_num] = 'free'

        if splitLine[0] == 'IFREE':
            inode_num = int(splitLine[1].split('\n')[0])
            inodeMap[inode_num] = 'free'

        if splitLine[0] == 'INODE':
            inodeNumber = int(splitLine[1])
            if inodeNumber not in inodeMap:
                inodeMap[inodeNumber] = 'allocated'
            elif inodeNumber in inodeMap:
                print("ALLOCATED INODE " + str(inodeNumber) + " ON FREELIST")
                inodeMap[inodeNumber] = 'allocated'

            for num in range(12, 27):
                blockNumber = int(splitLine[num].split('\n')[0])
                if num < 24 and blockNumber not in blockMap and blockNumber != 0:
                    if block_handler(blockNumber, inodeNumber, 0, first_nonreserved_block, block_count, 'BLOCK'):
                        blockMap[blockNumber] = (inodeNumber, 'BLOCK')
                elif num == 24 and blockNumber not in blockMap and blockNumber != 0:
                    if block_handler(blockNumber, inodeNumber, 12, first_nonreserved_block, block_count, 'INDIRECT BLOCK'):
                        blockMap[blockNumber] = (inodeNumber, 'INDIRECT BLOCK')
                elif num == 25 and blockNumber not in blockMap and blockNumber != 0:
                    if block_handler(blockNumber, inodeNumber, 268, first_nonreserved_block, block_count, 'DOUBLE INDIRECT BLOCK'):
                        blockMap[blockNumber] = (inodeNumber, 'DOUBLE INDIRECT BLOCK')
                elif num == 26 and blockNumber not in blockMap and blockNumber != 0:
                    if block_handler(blockNumber, inodeNumber, 65804, first_nonreserved_block, block_count, 'TRIPLE INDIRECT BLOCK'):
                        blockMap[blockNumber] = (inodeNumber, 'TRIPLE INDIRECT BLOCK')
                elif blockNumber != 0 and blockMap[blockNumber] == 'free':
                    print("ALLOCATED BLOCK " + str(blockNumber) + " ON FREELIST")
                elif num < 24 and blockNumber != 0 and blockMap[blockNumber] != 'free':
                    print("DUPLICATE BLOCK " + str(blockNumber) + " IN INODE " + str(inodeNumber) + " AT OFFSET 0")
                    print("DUPLICATE " + blockMap[blockNumber][1] + " " + str(blockNumber) + " IN INODE " + str(blockMap[blockNumber][0]) + get_offset(blockMap, blockNumber))
                elif num == 24 and blockNumber != 0 and blockMap[blockNumber] != 'free':
                    print("DUPLICATE INDIRECT BLOCK " + str(blockNumber) + " IN INODE " + str(inodeNumber) + " AT OFFSET 12")
                    print("DUPLICATE " + blockMap[blockNumber][1] + " " + str(blockNumber) + " IN INODE " + str(
                        blockMap[blockNumber][0]) + get_offset(blockMap, blockNumber))
                elif num == 25 and blockNumber != 0 and blockMap[blockNumber] != 'free':
                    print("DUPLICATE DOUBLE INDIRECT BLOCK " + str(blockNumber) + " IN INODE " + str(inodeNumber) + " AT OFFSET 268")
                    print("DUPLICATE " + blockMap[blockNumber][1] + " " + str(blockNumber) + " IN INODE " + str(
                        blockMap[blockNumber][0]) + get_offset(blockMap, blockNumber))
                elif num == 26 and blockNumber != 0 and blockMap[blockNumber] != 'free':
                    print("DUPLICATE TRIPLE INDIRECT BLOCK " + str(blockNumber) + " IN INODE " + str(inodeNumber) + " AT OFFSET 65804")
                    print("DUPLICATE " + blockMap[blockNumber][1] + " " + str(blockNumber) + " IN INODE " + str(
                        blockMap[blockNumber][0]) + get_offset(blockMap, blockNumber))

        if splitLine[0] == 'INDIRECT':
            inodeNumber = int(splitLine[1])
            layerNumber = int(splitLine[2])
            offset = int(splitLine[3])
            blockNumber = int(splitLine[5].split('\n')[0])

            if blockNumber in blockMap:
                if blockMap[blockNumber] == 'free':
                    print("ALLOCATED BLOCK " + str(blockNumber) + " ON FREELIST")
                elif blockMap[blockNumber] != 'free':
                    if layerNumber == 1:
                        print("DUPLICATE INDIRECT BLOCK " + str(blockNumber) + " IN INODE " + str(inodeNumber) + " AT OFFSET 12")
                    elif layerNumber == 2:
                        print("DUPLICATE DOUBLE INDIRECT BLOCK " + str(blockNumber) + " IN INODE " + str(inodeNumber) + " AT OFFSET 268")
                    elif layerNumber == 3:
                        print("DUPLICATE TRIPLE INDIRECT BLOCK " + str(blockNumber) + " IN INODE " + str(inodeNumber) + " AT OFFSET 65804")
            elif layerNumber == 1:
                if block_handler(blockNumber, inodeNumber, offset, first_nonreserved_block, block_count, 'INDIRECT BLOCK'):
                    blockMap[blockNumber] = (inodeNumber, 'INDIRECT BLOCK')
            elif layerNumber == 2:
                if block_handler(blockNumber, inodeNumber, offset, first_nonreserved_block, block_count, 'DOUBLE INDIRECT BLOCK'):
                    blockMap[blockNumber] = (inodeNumber, 'DOUBLE INDIRECT BLOCK')
            elif layerNumber == 3:
                if block_handler(blockNumber, inodeNumber, offset, first_nonreserved_block, block_count, 'TRIPLE INDIRECT BLOCK'):
                    blockMap[blockNumber] = (inodeNumber, 'TRIPLE INDIRECT BLOCK')

    for num in range(first_nonreserved_block, block_count):
        if num not in blockMap:
            print("UNREFERENCED BLOCK " + str(num))

    for num in range(first_nonreserved_inode, inode_count + 1):
        if num not in inodeMap:
            print("UNALLOCATED INODE " + str(num) + " NOT ON FREELIST")

    summary.seek(0)
    return inodeMap


def directAudit(summary, inodemap):
    parentMap = {}
    inodeLink = {}
    inodeLink_count = {}
    freeInode = []
    alocInode = []

    for inode in inodemap:
        if inodemap[inode] == 'free':
            freeInode.append(inode)
        else:
            alocInode.append(inode)

    for line in summary:
        splitLine = line.split(',')

        if splitLine[0] == 'INODE':
            inodeNumber = int(splitLine[1])
            linkCount = int(splitLine[6])
            inodeLink[inodeNumber] = linkCount
            inodeLink_count[inodeNumber] = 0

    summary.seek(0)

    for line in summary:
        splitLine = line.split(',')

        if splitLine[0] == 'DIRENT':
            parent = int(splitLine[1])
            reference = int(splitLine[3])
            name = str(splitLine[6].split('\n')[0])
            if reference in freeInode:
                print("DIRECTORY INODE " + str(parent) + " NAME " + str(name) + " UNALLOCATED INODE " + str(reference))
            elif reference not in freeInode and reference not in alocInode:
                print("DIRECTORY INODE " + str(parent) + " NAME " + str(name) + " INVALID INODE " + str(reference))
            elif name == "'.'" and reference != parent:
                print("DIRECTORY INODE " + str(parent) + " NAME " + str(name) + " LINK TO INODE " + str(reference) + " SHOULD BE " + str(parent))
            else:
                inodeLink_count[reference] += 1
                if reference not in parentMap:
                    parentMap[reference] = parent

    summary.seek(0)

    for line in summary:

        splitLine = line.split(',')

        if splitLine[0] == 'DIRENT':
            parent = int(splitLine[1])
            reference = int(splitLine[3])
            name = str(splitLine[6].split('\n')[0])
            if name == "'..'":
                g_parent = parentMap[parent]
                if reference != g_parent:
                    print("DIRECTORY INODE " + str(parent) + " NAME " + str(name) + " LINK TO INODE " + str(reference) + " SHOULD BE " + str(g_parent))

    for inode in inodeLink:
        if inodeLink[inode] != inodeLink_count[inode]:
            print("INODE " + str(inode) + " HAS " + str(inodeLink_count[inode]) + " LINKS BUT LINKCOUNT IS " + str(inodeLink[inode]))


def block_handler(blockNumber, inodeNumber, offset, start, end, layer):

    if blockNumber < 0 or blockNumber > end - 1:
        print("INVALID " + str(layer) + " " + str(blockNumber) + " IN INODE " + str(inodeNumber) + " AT OFFSET " + str(offset))
        return False
    elif blockNumber in range(1, start - 1):
        print("RESERVED " + str(layer) + " " + str(blockNumber) + " IN INODE " + str(inodeNumber) + " AT OFFSET " + str(offset))
        return False
    return True


def get_offset(blockMap, blockNumber):
    t = str(blockMap[blockNumber][1])
    if t == 'BLOCK':
        return " AT OFFSET 0"
    elif t == 'INDIRECT BLOCK':
        return " AT OFFSET 12"
    elif t == 'DOUBLE INDIRECT BLOCK':
        return " AT OFFSET 268"
    elif t == 'TRIPLE INDIRECT BLOCK':
        return " AT OFFSET 65804"


def run(csv):

    try:
        summary = open(csv)
        inodeMap = Audit(summary)
        directAudit(summary, inodeMap)

    except IOError:
        sys.stderr.write('cannot open file\n')
        exit(1)


def main():

    if len(sys.argv) == 1:
        sys.stderr.write('cannot find file input\n')
        exit(1)

    for argument in sys.argv:
        if argument == 'lab3b.py':
            continue
        run(argument)


if __name__ == '__main__':
    main()


