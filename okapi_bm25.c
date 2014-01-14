#include <math.h>
#include <sqlite3ext.h>
#include <assert.h>
SQLITE_EXTENSION_INIT1

static void okapi_bm25(sqlite3_context *pCtx, int nVal, sqlite3_value **apVal) {
    assert(sizeof(int) == 4);

    unsigned int *matchinfo = (unsigned int *)sqlite3_value_blob(apVal[0]);
    int searchTextCol = sqlite3_value_int(apVal[1]);

    double K1 = ((nVal >= 3) ? sqlite3_value_double(apVal[2]) : 1.2);
    double B = ((nVal >= 4) ? sqlite3_value_double(apVal[3]) : 0.75);

    int P_OFFSET = 0;
    int C_OFFSET = 1;
    int N_OFFSET = 2;
    int A_OFFSET = 3;

    int termCount = matchinfo[P_OFFSET];
    int colCount = matchinfo[C_OFFSET];

    int L_OFFSET = (A_OFFSET + colCount);
    int X_OFFSET = (L_OFFSET + colCount);

    double totalDocs = matchinfo[N_OFFSET];
    double avgLength = matchinfo[A_OFFSET + searchTextCol];
    double docLength = matchinfo[L_OFFSET + searchTextCol];

    double sum = 0.0;

    for (int i = 0; i < termCount; i++) {
        int currentX = X_OFFSET + (3 * searchTextCol * (i + 1));
        double termFrequency = matchinfo[currentX];
        double docsWithTerm = matchinfo[currentX + 2];

        double idf = log(
            (totalDocs - docsWithTerm + 0.5) /
            (docsWithTerm + 0.5)
        );

        double rightSide = (
            (termFrequency * (K1 + 1)) /
            (termFrequency + (K1 * (1 - B + (B * (docLength / avgLength)))))
        );

        sum += (idf * rightSide);
    }

    sqlite3_result_double(pCtx, sum);
}

int sqlite3_extension_init(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi) {
    SQLITE_EXTENSION_INIT2(pApi)

    sqlite3_create_function(db, "okapi_bm25", -1, SQLITE_ANY, 0, okapi_bm25, 0, 0);
    return 0;
}
