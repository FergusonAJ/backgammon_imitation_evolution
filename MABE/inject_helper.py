L = []
with open('makefile', 'r') as in_fp:
    for line in in_fp:
        L.append(line)

with open('makefile', 'w') as out_fp:
    for line in L:
        tmp = line.split(' ')
        if 'c++' in tmp[0] and 'Backgammon' in tmp[-1]:
            out_fp.write(line[:-1])
            out_fp.write(' -I ${EMP_SRC}\n')
        else:
            out_fp.write(line) 
         
