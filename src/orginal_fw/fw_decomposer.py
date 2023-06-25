import sys

class FwDecompozer:
   def __init__(self, vector_table_size, vector_table_address, file):
      self.vector_table_size = vector_table_size * 4
      self.vector_table_address = vector_table_address
      self.file = open(file, 'rb')

   def save_vector_table(self, filename):
      output = open(filename, 'wb')
      self.file.seek(self.vector_table_address, 0)
      output.write(self.file.read(self.vector_table_size))
      output.close()

   def save_part1(self, filename):
      output = open(filename, 'wb')
      self.file.seek(0)
      output.write(self.file.read(self.vector_table_address))
      output.close()

   def save_part2(self, filename):
      output = open(filename, 'wb')
      self.file.seek(self.vector_table_address + self.vector_table_size, 0)
      output.write(self.file.read())
      output.close()

if __name__ == '__main__':
   args = sys.argv
   fw = FwDecompozer(int(args[1]), int(args[2]), args[3])
   fw.save_vector_table(args[4])
   fw.save_part1(args[5])
   fw.save_part2(args[6])