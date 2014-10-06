# (3,2) znamena, ze mamae v ADJ. LIST tu samou hranu 2krat

# vrchol_index -> [(index_sousedniho_vrcholu, index_hrany)]

# index_hrany -> (cena hrany, [seznam indexu sousdnich hran])

filename = 'ex2.in'
f = open(filename, 'r')
lines = f.readlines()
parsed = [int(elem) for elem in lines[0].split()]
N, M = parsed[0], parsed[1]


edges    = [None for i in range(M)]
adj_list = [[] for i in range(N)]


for index, line in enumerate(lines[1:]):
    values = [int(v) for v in line.split()]
    edge_price = values[2]
    a,b = values[0], values[1]
    edges[index] = {"index": index, 
                    "coords": (a,b),
                    "price": edge_price, 
                    "neighboors": {a: None, b: None},
                    "best_path_price" : {a: None, b: None}} # pro kazdou hranu ulozime cenu

    # v C++ pouzit tuple<5>
    # misto mapovani, a->..., b->..., leve_sousedi, prave_sousedi (podle leveho/praveho indexu)
    # stejne pak, nejlepsi cesta zleva/zprava
    adj_list[a].append((b,index))
    adj_list[b].append((a,index))





print('')
print('ADJ. LIST...')
for v in adj_list:
    print(v)

# 1 - sestavit pro kazdou hranu seznam vsech sousednich hran s MENSI delkou

for curr_e_index, e in enumerate(edges):
    a,b = e["coords"] # ziskame poc. a koncove vrcholy dane hrany
    curr_price = e["price"]
    a_out_edges = [(edge_index, edges[edge_index]['price']) \
                    for _, edge_index in adj_list[a] \
                    if edge_index != curr_e_index and edges[edge_index]['price'] < curr_price]
    b_out_edges = [(edge_index, edges[edge_index]['price']) \
                    for _, edge_index in adj_list[b] \
                    if edge_index != curr_e_index and edges[edge_index]['price'] < curr_price]

    e["neighboors"][a] = a_out_edges
    e["neighboors"][b] = b_out_edges

# sort all edges by price
edges_by_price = sorted(edges, key=lambda e: e['price'])
# pouzit pole ukazatelu a seradit ho v C++

global_max = -1

for e in edges_by_price:
    a,b = e["coords"]

    # protoze v C++ nebudu mit mapu, je potreba zajistit
    # kterou vetev brat - pravou nebo levou (misto `a` a `b`)
    # if .... then `a` else `b`
    # pamatuj, ze my tady mame INVERZNI poradi cest



    best_right_neighbr = [edges[n[0]]['best_path_price'][b] \
                                for n in e['neighboors'][b]]

    best_left_neighbr  = [edges[n[0]]['best_path_price'][a] \
                                for n in e['neighboors'][a]]
    p_r = max(best_right_neighbr or [0])
    p_l = max(best_left_neighbr or [0])
    e['best_path_price'][a] = e['price'] + p_r
    e['best_path_price'][b] = e['price'] + p_l

    best_res = max(e['best_path_price'][a], e['best_path_price'][b])

    if best_res > global_max:
        global_max = best_res

print('EDGES...')
for e in edges_by_price:
    print(e)

print('best result: '+str(global_max))

f.close()