# JNP1

**Języki i narzędzia programowania 1**, MIMUW 

**Languages and tools for programming I**, University of Warsaw, Faculty of Mathematics, Informatics and Mechanics
___
## Treści zadań
Treści zadań znajdują się w [tutaj](/common).

## Zadanie 1.
Program wspomagający pracę kasy biletowej. Używa rozwiązania sześciennego, aby znaleźć optymalny set biletów. Do obsługi wejścia używane są regexy.
Rozwiązanie znajduje się w folderze [zadanie_pierwsze](/zadanie_pierwsze).

## Zadanie 2.
Moduł obsługujący zbiory częściowo uporządkowane (ang. poset), których elementami są napisy. Moduł ma być używany w C.
Rozwiązanie znajduje się w folderze [zadanie_drugie](/zadanie_drugie).


## Obsługa svn:

Dodawanie pliku do repo:
```
svn import nazwa_pliku.cc https://svn.mimuw.edu.pl/repos/jnp1-students/grupa3/zadanienumer/folder/nazwa_pliku.cc -m "Dodano rozwiązanie"
```
Pobieranie repo(na przykład treści zadań): 
```
svn checkout https://svn.mimuw.edu.pl/repos/jnp1-students/common --username as406384
```
Update repo:
``` 
svn update
```
