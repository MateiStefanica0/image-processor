    Link tema:
        https://ocw.cs.pub.ro/courses/programare/teme_2023/tema1_2023_cbd
                
                TEMA 1 - STEFANICA MATEI COSTIN - 311CB

        IMAGEPROCESSING.C   

    Toate taskurile din acest fisier sunt executate element cu element al matricei, desi nu este nevoie de fiecare data, deoarece mi s-a parut mai usor de urmarit si lucrat asa.
    FLIP HORIZONTAL - interschimbare a valorilor din capetele randurilor matricei catre interior, pana la jumate
    ROTATE LEFT - realizam operatia prin "horizontal flip" + transpunere
    CROP - cream o matrice noua, pe care o umplem doar cu partea din matricea veche care ne intereseaza, iar pe cea veche o eliberam
    EXTEND - cream o matrice noua, pe care o umplem cu noile valori RGB, iar apoi "suprapunem" peste ea, centrat, valorile din vechea matrice 
    PASTE - avem nevoie de 2 variable: stopN si stopM, pe care le initializam cu minimurile dimensiuilor celor 2 matrice, la care adaugam x, respectiv y. Apoi doar incepem sa copiem din matricea sursa, incepand cu punctul de coordonate (x, y), pana la stopN si stopM
    APLLY FILTER - cream o matrice noua. Pe cea veche o extindem cu rezultatul impartirii intregi a dimensiunii filtrului la 2 si cu valorile 0, 0, 0. Astfel, vom avea spatiul necesar pe conturul imaginii pentru a putea aplica filtrul. Pentru fiecare "pixel" din imagine, parcurgem matricea patratica (n = filter_size) centrata in pixelul respectiv, iar pentru fiecare dintre cele 3 valori aplicam formula de calcul a filtrului. Dupa aceea, comparam rezultatele cu 0 (min) si 255 (max), pentru a ne asigura ca rezultatele finale vor apartine intervalului [0, 255]. La final, rotunjim (prin cast) rezultatele la valori intregi.

        INTERACTIVE.C   
    Pe fiecare ramura "else if" exista cate un comentariu cu parametrii care trebuie sa fie cititi si unul cu antetul functiei folosite, daca este cazul
    - definim valorile constante pentru alocari
    - cream cate o structura pentru Poze si pentru Filtre
    - alocam ca default cate 100 de pozitii atat pentru poze, cat si pentru filtre. In cazul in care sunt incarcate mai multe, variabilele memory_increase_poze si memory_increase_filtre vor fi incrementate, iar pointerii aferenti realocati, cu 100 de pozitii in plus. Aceasta operatie poate fi facuta de oricate ori este nevoie, desi nu exista scenariu in filtrele checkerului in care sa fie necesar
    - pentru intreg procesul de rulare a programului si a asteptarii de comenzi de la utilizator am folosit while(1), cu singura conditie de a iesi din bucla fiind cazul in care se primeste comanda 'e'. Astfel, in cazul in care o comanda necunoscuta este folosita, ea va fi pur si simplu ignorata

    "e" - eliberam toata memoria alocata pentru poze si filtre, iar apoi returnam 0 pentru a incheia executia programului
    "l" - alocam memorie pentru o noua poza, folosim functie read_from_bmp pentru a obtine matricea cu care putem lucra si incrementam numarul numarul de poze. In cazul in care numarul de poze depaseste memoria alocata initial, incrementam variabila memory_increase_poze si realocam pointerul poze, cu inca 100 de pozitii
    "s" - folosim functia write_to_bmp
    "ah" - folosim functia flip_horizontal
    "ar" - folosim functia rotate_left si interschimbam dimensiunile pozei
    "ac" - folosim functia crop si modificam dimensiunile pozei cu valorile h si w
    "ae" - folosim functia extend si marim dimensiunile pozei si 2 * rows, respectiv cols
    "ap" - folosim functia paste
    "cf" - alocam memorie pentru valorile unui nou filtru, pe care le citim si apoi incrementam numarul de filtre din memorie. In cazul in care numarul de filtre depaseste capacitatea initiala, incrementam variabila memory_increase_filtre si realocam pointerul filtre cu 100 de pozitii in plus
    "af" - folosim functia apply_filter
    "df" - eliberam memoria alocata pentru filtrul pe care trebuie sa il stergem, mutam toate filtrele incepand cu pozitia respectiva, cu o pozitie mai la stanga, decrementam variabila nr_filtre si, la final, eliberam memoria alocata pentru ultimul filtru, care acum a ramas in plus
    "di" - eliberam memoria alocata pentru poza pe care trebuie sa o stergem, mutam toate pozele incepand cu respectiva pozitie, cu o pozitie mai la stanga, decrementam variabila nr_poze si, la final, eliberam memoria alocata pentru ultimul filtru