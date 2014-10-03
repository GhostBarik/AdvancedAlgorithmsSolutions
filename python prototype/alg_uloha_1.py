# nacist N,D

# nacist hrany, pro kazdou hranu spocitat delku a ulozit

# hrana (a,b), delka(len), seznam vsech hran (indexy), ktere ji protinaji
# (to lepe udelat pres matice(C++),
# abychom meli O(1) rychlost odpovedi)

# pro kazdou hranu spocitat seznam vsech hran, ktere protina
# aby se nenastala situace, kdy zjistujeme jestlize protina nebo ne 2krat
# (napr. nejprve A a B, ale pak pro B a A), meli bychom nejak
# zaindexovat ti vysledky

# a posledni vec, vsak velice dulezita:
# sestavit adj. list pro kazdy vrchol a SERADIT hrany podle delky (od nejmensi)

# 2 cast:
# ukladat delku u hrany primo bez pouziti matice

# 3 cast: ADJ. LIST reprezentace

# 4 cast
# BASIC ALGORITHM
#
# -) zacneme z jakehokoliv vrcholu
# 1) prochazime seznam hran (min len...max len) ()
#    (a) udrzujeme v pameti mnozinu jiz pridanych hran (OK)
#        alternativa: udrzovat mnozinu hran, se kterymi se krizi
#        jiz pridane hrany. Pokud nova hrana, kterou chceme pridat
#        do teto mnoziny patri => ignorujeme ji a jdeme dal
#    (b) pokud hrana je OK a splnujeme dalsi podminky -> pridame ji
#        to znamena predevsim UPDATNOUT mnozinu "spatnych" hran (sjednoceni)
# 2) rekurzivne pokracujeme...
# 3) na konci kazdeho kroku (po pridani nove hrany) muzeme zkusit
#    uzavrit cyklus a tim pripadne updatnout dosud nejlespi nalezeny vysledek

# datove struktury, co z toho plynou:
# 1) ADJ. LIST, kde kazdy vrchol obsahuje SERAZENY seznam sousednich hran
#   (od nejkratsi k nejdelsi) -> indexujeme hrany podle dvojici (start,end)




import math
import itertools

import intersection 

N = 13
D = 4400

points1 =\
"""100 800
200 400
300 1200
400 900
500 100
500 500
600 700
700 1100
800 500
900 300
900 1000
1100 700
1100 1100
"""

# N = 4
# D = 2000

# points1 =\
# """0 0
# 0 500
# 500 500
# 500 0"""

lines = points1.split('\n')

# nacteme vrcholy ze vstupu
vertices = [tuple(int(i) for i in line.split()) \
                      for line in lines]

def vecLength(x1,y1,x2,y2):
    return math.sqrt((x2-x1)**2 + (y2-y1)**2)



# vytvorime matice hran, kde zatim v kazde bunce budeme ukladat delku

# optimalizace: nepouzivat objekty, staci tuple
class Edge(object):
    def __init__(self, l, i):
        self.length = l
        self.index = i

edges = [[0 for x in range(N)] for y in range(N)] # matice N*N


# seznam hran, reprezentovany pomoci seznamu dvojic indexu vrcholu
# (SERAZEN LEXIKOGRAFYCKE, vzdy plati, ze (a,b) => a < b)
edges_list = [(row,column) for row in range(N) for column in range(row+1,N)]


# spocitame matice hran (|V*V|)
# radek - vrchol 1
# sloupec - vrchol 2
index_counter = 0

for row, column in edges_list:
        v1 = vertices[row]
        v2 = vertices[column]
        l = vecLength(v1[0], v1[1], v2[0], v2[1])
        new_edge = Edge(l, index_counter)
        index_counter += 1
        edges[row][column] = new_edge
        edges[column][row] = new_edge
        


# celkovy pocet hran
E = N*(N-1)//2

# nalezneme vsechne mozne kombinace 2 hran
edg_comb         = list(itertools.combinations(edges_list, 2))
edg_indexes_comb = list(itertools.combinations(range(E),   2))

# matice vsech pruseciku
# kazda bunka obsahuje zaznam `True` nebo `False`
# indexovana podle hran => 0..E-1
edge_crossings = [[True for x in range(E)] for y in range(E)] # matice E*E

# ve verzi pro C++ dalo by se nejak sjednotit generovani
# indexu a kombinaci ze seznamu hran, abychom zbytecne nezipovali atd.
for (e1, e2),(e_row,e_col) in zip(edg_comb, edg_indexes_comb):
    # vytahneme souradnice jednotlivych vrcholu

    e1_v1_x = vertices[e1[0]][0]
    e1_v1_y = vertices[e1[0]][1]
    e1_v2_x = vertices[e1[1]][0]
    e1_v2_y = vertices[e1[1]][1]

    e2_v1_x = vertices[e2[0]][0]
    e2_v1_y = vertices[e2[0]][1]
    e2_v2_x = vertices[e2[1]][0]
    e2_v2_y = vertices[e2[1]][1]

    inter = \
        intersection.intersected(e1_v1_x, e1_v1_y, e1_v2_x, e1_v2_y,
                                 e2_v1_x, e2_v1_y, e2_v2_x, e2_v2_y)

    edge_crossings[e_row][e_col] = inter
    edge_crossings[e_col][e_row] = inter # mirroring


#bin_crossings = [int(j) for i in edge_crossings for j in i]

#print(bin_crossings)

# tedka mame 2 tabulky => delky a indexy hran(V*V) a testu_protinani(E*E)

# pro kazdou dvojici vrcholu se da v O(1) spocitat index odpovidajici
# hrany z tabulky testu



best_cycle_num_vert = -1 # >> MAX
best_price = 99999999    # >> MIN


start_v = 3


def recur(v, path_price, bad_edges, visited_v, depth):

    global best_cycle_num_vert, best_price

    #############

    # oznacime vrchol jako navstiveny
    new_visited = visited_v | {v} # O(1)

    for i, e in enumerate(edges[v]):
        if i == v or (i in visited_v): # vrchol nemuze mit hranu sam s sebou
            # zkusme uzavrit cyklus
            if i == start_v and depth > 0: # pridat jeste dodatecnou optimalizacni podminku!!!
                total_price = e.length + path_price
                total_vertices = depth + 1

                # je to lepsi nez dosad' nalezeny vysledek?
                if ((total_vertices > best_cycle_num_vert) or\
                   (total_vertices == best_cycle_num_vert and total_price < best_price)) and\
                   total_price <= D:
                    # v pripade ze mame stejny pocet vrcholu ale LEVNEJSI cestu
                    best_cycle_num_vert = total_vertices
                    best_price = total_price

            continue   # a nesmi byt navstiveny

        # TESTY NA HRANU
        if e.index in bad_edges: # O(1) pomoci bitset
            continue # ignorujeme

        # obnovime cenu cesty
        new_price = path_price + e.length

        # nepresahli jsme nahodou D?
        if new_price > D:
            continue
            
        # obnovime mnozinu "spatnych"
        invalid_edges = [i for i,v in enumerate(edge_crossings[e.index]) if v == True]
        new_bad_edges = bad_edges.union(invalid_edges) # sjednoceni 2 mnozin, skoro O(1)
        # v C++ pouzijeme rychly bitset!

        recur(i, new_price, new_bad_edges, new_visited, depth+1)


recur(start_v, 0, set(), set(), 0)

print("best total vertices: "+str(best_cycle_num_vert))
print("best price: "+str(best_price))