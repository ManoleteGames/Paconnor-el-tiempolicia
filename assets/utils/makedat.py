import sys
import os

def PackFiles(file_list, dat_file):
    
    print("- Starting pack files -")
    file_offset = 0 #target file cursor
    file_allocation = 2080 #first file start allocation    

    with open(dat_file, 'wb') as f:
        #1st. Print header
        print("- Generating dat file -")
        f.write(b'DAT PACK FILE BY J.MARTIN')
        file_offset += 32
        f.seek(file_offset,os.SEEK_SET)
        
        #2nd Print file names, sizes, offsets and data
        for file in file_list:            
            
            #print("- Processing file ", file)
            #filename            
            f.write(file.encode('utf-8'))
            #file_offset += 14
            #f.seek(file_offset,os.SEEK_SET)
            #size, and location inside dat file            
            #size = os.path.getsize(file)
            #print("-- file size ",size)
            #f.write(size.to_bytes(6,'big'))
            
            #start allocation
            file_offset += 16
            f.seek(file_offset,os.SEEK_SET)
            f.write(file_allocation.to_bytes(4,'little'))

            #end allocation
            file_offset += 4
            f.seek(file_offset,os.SEEK_SET)
            size = os.path.getsize(file)
            f.write(size.to_bytes(4,'little'))
            
            #set offset for new file
            file_offset += 12

            #data
            print("-- allocating file data on offset ",file_allocation)
            with open(file, 'rb') as data:
                f.seek(file_allocation,os.SEEK_SET)
                f.write(data.read())
            
            #recalculate next file allocation
            file_allocation = file_allocation + size
            f.seek(file_offset,os.SEEK_SET)

print("*** DAT FILE PACKING PROCESS ***")
print("- Total files to pack:", len(sys.argv)-1)
if(len(sys.argv)<3):
    sys.exit("[ERROR] No files to pack")
if(len(sys.argv)>32):
    sys.exit("[ERROR] Maximum files to pack 32!")
print("- Target file:", sys.argv[1])
print("- Files to pack:", sys.argv[2:])
file_list = sys.argv[2:]
PackFiles(file_list, sys.argv[1])















     
