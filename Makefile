SRC := okapi_bm25.c
OUT := okapi_bm25.sqlext

$(OUT): $(SRC)
	gcc -Wall -Werror -bundle -fPIC -Isqlite3 -o $(OUT) $(SRC)

clean:
	rm -f $(OUT)
