CC= cc -std=c99
CFLAGS= -O2 -pedantic -Wall -Wextra -Wno-c11-extensions $G

L= pdfsg
P= glyph
O= output

all:	$P
	./$P
	open $O.pdf

$P:	$P.c $L.c $L.h
	$(CC) $(CFLAGS) -o $P $P.c $L.c $(LIBS)

diff:
	-diff etc/$L.h .
	-diff etc/$L.c .
	-diff etc/$P.c .

save:
	cp -p $P.c $L.c $L.h $O.pdf etc

clean:
	-rm -f $P $O.pdf a.out *.o *.so

h:
	grep '^[a-z].*)$$' $L.c | grep -v static | sed 's/$$/;/' > $L.h
