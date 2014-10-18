

filename = 'test1.txt'
f = open(filename, 'r')

lines = f.readlines()

# nacteme prvni radek
parsed = [int(elem) for elem in lines[0].split()]
M, N, E, F, S, T, *rest = parsed

# S - start, T - end
# M - sities with Oyster, N - without

V = M+N

# inicializace struktur
edges    = [None for i in range(E)]
adj_list = [[] for i in range(V)]
oyster_pumps = {}

# C++ : pouzit normalni strukturu misto slovniku
vert_data = [{"best_path": 99999999999, # dodatecne udaje pro kazdy vrchol
              "oyster_city": False,
              "fuel_left": -1,
              "visiting_time": -1} for i in range(V)]

# mnozina jiz navstivenych mest
visited_set = set() # bitset?
max_fuel_capacity = F

# nacteme indexy mest, obsahujicich Oyster pumpy
for i in range(M):
    v_index = int(lines[1+i])
    oyster_pumps[v_index] = {} # neighboor_pumps ## DULEZITE: mam opravdu tady pouzit MAP (asi jo)
    # updatneme informace o danem meste
    v_data = vert_data[v_index]
    v_data["fuel_left"]   = max_fuel_capacity
    v_data["oyster_city"] = True


# uplne stejne v C++!
# nacteme seznam hran
for line in lines[(1+M):]:
    parsed = [int(elem) for elem in line.split()]
    a, b, w = parsed
    # pridame hranu do seznamu sousednosti
    adj_list[a].append((b,w))
    adj_list[b].append((a,w))


# prioritni fronta, kam budeme ukladat
# vrcholy serazene podle nejlepsi cesty
import heapq
Q = []
# C++ -> pouzit existujici nebo naimplementovat svoji vlastni?


# JINY NAVRH:
# 1) spustit dijkstru s kazde pumpy (a tim spocitat nejlepsi vzdalenosti mezi pumpami)
# 2) najit min. cestu pri prochazeni pumpami


def Dijkstra(start_pump, time, final=False):
    # nastavime cenu startovniho vrcholu na 0
    # aby algoritmus z neho zacal
    vert_data[start_pump]["best_path"] = 0
    # zaciname s max. poctem paliva
    vert_data[start_pump]["fuel_left"] = max_fuel_capacity

    # vlozime poc. vrchol do fronty
    heapq.heappush(Q, (vert_data[start_pump]["best_path"], start_pump))
    
    # pridame vsechny vrcholy do prioritni fronty
    # for v_index in range(V):
    #     if v_index != S:            

    # dokud fronta neni prazdna -> vytahneme dalsi vrchol
    while len(Q) != 0:
        _,u = heapq.heappop(Q)
        # oznacime vrchol za "navstiveny"
        visited_set.add(u)

        # teoreticky to muzme nechat pro finalni pruchod
        # -> muze to trochu zrychlit alg.
        # if u == T: # je to konecny uzel?
        #     return 

        # vytahneme dodatecne udaje pro `u`
        u_data = vert_data[u]

        # prochazime/zkoumame sousedy
        # C++ mapa vs. list ??? -> reseni: sablony?
        neighb = oyster_pumps[u].items() if final else adj_list[u]

        for v, price_to_v in neighb:

            v_data = vert_data[v]

            # mame dostatek paliva?
            if (u_data["fuel_left"] < price_to_v and not final):
                continue # nedojedeme :(

            # spocitame delku nove cesty
            new_path_price = u_data["best_path"] + price_to_v
            new_fuel_left  = 0 if v_data["oyster_city"]\
                               else u_data["fuel_left"] - price_to_v

            # "fresh" -> cerstvy uzel, ktery jsme jeste nenavstevovali
            fresh = (v_data["visiting_time"] < time)

            if v not in visited_set: # pokud vrchol neni zavreny

                # pokud je lepsi/mensi -> obnovime informace v danem vrcholu
                if new_path_price < v_data["best_path"] or fresh:

                    if v_data["oyster_city"]: 
                        oyster_pumps[start_pump][v] = new_path_price
                        oyster_pumps[v][start_pump] = new_path_price

                    # update best path for v
                    old_price = v_data["best_path"]
                    v_data["best_path"] = new_path_price
                    v_data["visiting_time"] = time
                    v_data["fuel_left"] = new_fuel_left

                    if fresh:
                        heapq.heappush(Q, (v_data["best_path"], v))
                    else:
                        # updatovanim cesty zmenili jsme prioritu daneho vrcholu
                        # ve fronte, takze musime ji(frontu) take updatenout
                        Q.remove((old_price, v))
                        heapq.heapify(Q) # pomala verze DECREASE(key)
                        heapq.heappush(Q, (v_data["best_path"], v))
                        # OPTIMALIZACE: naimplementovat svoji vlastni 
                        # prioritni frontu, podporujici DECREASE(KEY)?


for i,k in enumerate(oyster_pumps):
    time = i
    visited_set = set()
    Dijkstra(k, time)

time += 1
visited_set = set() # empty()
Dijkstra(S, time, final=True)

# final Dijkstra!


# for k,v in oyster_pumps.items():
#     print("{} => {}".format(k,v))

# testy
# print('')
# print('ADJ. LIST...')
# for vert in adj_list:
#     print(vert)

# print()
# print('VERTEX DATA...')
# for vert in vert_data:
#     print(vert)


print("best: "+str(vert_data[T]["best_path"]))









