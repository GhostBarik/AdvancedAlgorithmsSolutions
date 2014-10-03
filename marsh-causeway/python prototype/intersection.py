# Module intersection.py

# description: modul obsahuje funkci pro vypocet pruseciku mezi 2 usecky
# algoritmus je vzat odsad' -> http://stackoverflow.com/questions/
# 563198/how-do-you-detect-where-two-line-segments-intersect
 

# author: Daniel Barabash
# date:   30.9.2014


# vektorovy soucin pro 2d vektory
def cross2D(x1, y1, x2, y2):
    return x1*y2 - x2*y1 # inline

def dot2D(x1, y1, x2, y2):
    return x1*x2 + y1*y2 # inline


# test na prusecik (True - existuje / False - neexistuje)
# paramtetry => body p1,p2,q1,q2
# mozne optimalizace: inline
def intersected(p1_x, p1_y, p2_x, p2_y, q1_x, q1_y, q2_x, q2_y):
    
    # p (p1_x,p1_y) -> (p2_x, p2_y)
    # q (q1_x,q1_y) -> (q2_x, q2_y)

    # p = p_s + t * r
    # q = q_s + u * s

    # smerovy vektor pro p
    r_x = p2_x - p1_x
    r_y = p2_y - p1_y

    # smerovy vektor pro q
    s_x = q2_x - q1_x
    s_y = q2_y - q1_y

    # b = q - p (rozdilovy vektor b)
    b_x = q1_x - p1_x
    b_y = q1_y - p1_y

    # test1 => r `cross` s
    test1 = cross2D(r_x, r_y, s_x, s_y)

    # test2 => (q - p) `cross` r
    test2 = cross2D(b_x, b_y, r_x, r_y)

    if test1 == 0:
        if test2 == 0:
            testSS = dot2D(s_x, s_y, s_x, s_y)
            testRR = dot2D(r_x, r_y, r_x, r_y)
            testBR = dot2D(b_x, b_y, r_x, r_y)
            testBS = dot2D(b_x, b_y, s_x, s_y)

            if (0 <= testBR <= testRR) or (0 <= testBS <= testSS):
                #print("overlapping")
                return True
            else:
                #print("collinear and disjoint")
                return False
        else:
            #print("parallel and no-intersecting")
            return False
    else:
        # find `t` and `u`
        RS = cross2D(r_x, r_y, s_x, s_y)
        t = cross2D(b_x, b_y, s_x, s_y) / RS
        u = cross2D(b_x, b_y, r_x, r_y) / RS

        if (0 <= t <= 1) and (0 <= u <= 1):
            #print("intersected {} {}".format(t,u))
            if (t == 0 or t == 1) and (u == 0 or u == 1): # POZOR NA NUMERICKE CHYBY!
                return False
            return True
        else:
            #print("non parallel nor intersected")
            return False


#######################

# velice jednoduche testy na castecne otestovani korektniho
# chovani algoritmu

# problem: z wiki -> pokud usecky jsou "skoro" paralelni
# (jsou velice blizko ale jeste paralelita nenastava) muze dochazet
# k numerickym chybam

        
if __name__ == "__main__":
    # TEST CASES
    #print("running TEST CASES...")
    #print("...")

    # test1 -> overlapped vectors
    v1 = (0,0); v2 = (1,1)
    u1 = (0,0); u2 = (1,1)

    #print("test for overlapped: ", end="")
    assert(intersected(v1[0], v1[1], v2[0], v2[1],
                       u1[0], u1[1], u2[0], u2[1]) == True)

    # test2 -> parralel and no intersecting
    v1 = (0,0); v2 = (0,1)
    u1 = (1,0); u2 = (1,1)

    #print("parallel and no intersecting: ", end="")
    assert(intersected(v1[0], v1[1], v2[0], v2[1],
                       u1[0], u1[1], u2[0], u2[1]) == False)

    # test3 -> intersected vectors (a)
    v1 = (1,0);   v2 = (0.75,1)
    u1 = (0,0.5); u2 = (1,0.5)

    #print("intersected vectors: ", end="")
    assert(intersected(v1[0], v1[1], v2[0], v2[1],
                       u1[0], u1[1], u2[0], u2[1]) == True)


    # test4 -> intersected vectors (b)
    v1 = (0,0); v2 = (1,1)
    u1 = (0,1); u2 = (1,0)

    #print("intersected vectors: ", end="")
    assert(intersected(v1[0], v1[1], v2[0], v2[1],
                       u1[0], u1[1], u2[0], u2[1]) == True)


    # test5 -> collinear and disjoint vectors
    v1 = (0,0); v2 = (1,0)
    u1 = (3,0); u2 = (2,0)

    #print("collinear and disjoint vectors: ", end="")
    assert(intersected(v1[0], v1[1], v2[0], v2[1],
                       u1[0], u1[1], u2[0], u2[1]) == False)


    # test6 -> non-parallel nor intersected
    v1 = (0,0); v2 = (1,1)
    u1 = (1.5,1); u2 = (1,2)

    #print("non-parallel nor intersected: ", end="")
    assert(intersected(v1[0], v1[1], v2[0], v2[1],
                       u1[0], u1[1], u2[0], u2[1]) == False)

