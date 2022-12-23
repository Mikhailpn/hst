import random
import math
from sys import argv

def generate_matrix(size):
    matrix = []
    m = int(math.sqrt(size/3))
    for i in range(m):
        matrix.append([])
        for j in range(m):
            if random.randint(0, 1) == 0:
                el = random.randint(-9, -1)
            else:
                el = random.randint(10, 19)

            matrix[i].append(el)
    return (matrix)


def write_matrix(f, matr):
    size = str(len(matr))
    f.write(size + " " + str(len(matr[0])) + "\n")
    for row in matr:
        for el in row:
            f.write(str(el) + " ")
        f.write("\n")


def generate(size, file_name):
    f = open(file_name, "w")
    matrix = generate_matrix(size * 1024 * 1024)
    write_matrix(f, matrix)
    f.close()



script, file_name, size = argv
generate(int(size), file_name)





