import sys
class FwDecompozer:
   def __init__(self, vector_table_size, file):
      self.vector_table_size = vector_table_size * 4
      self.file = open(file, 'rb')

   def save_vector_table(self, filename):
      output = open(filename, 'wb')
      self.file.seek(0)
      output.write(self.file.read(self.vector_table_size))
      output.close()

   def save_stripped_fw(self, filename):
      output = open(filename, 'wb')
      self.file.seek(self.vector_table_size, 0)
      output.write(self.file.read())
      output.close()

if __name__ == '__main__':
   args = sys.argv
   fw = FwDecompozer(int(args[1]), args[2])
   fw.save_vector_table(args[3])
   fw.save_stripped_fw(args[4])