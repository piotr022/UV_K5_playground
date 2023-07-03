import sys

def merge_files(in1, in2, out):
    f1 = open(in1, 'rb')
    f2 = open(in2, 'rb')
    fo = open(out, 'wb')

    fo.write(f1.read())
    fo.write(f2.read())
    fo.close()
    f1.close()
    f2.close() 

if __name__ == '__main__':
   args = sys.argv
   merge_files(args[1], args[2], args[3])