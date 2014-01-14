var sqlite3 = require('sqlite3').verbose(),
    test = require('tape');

var db = new sqlite3.Database(__dirname + '/test.db');
db.loadExtension(__dirname + '/../okapi_bm25.sqlext');

test('calculates correct scores', function(t) {
  t.plan(4);

  var query = "SELECT id, okapi_bm25(matchinfo(songs, 'pcnalx'), 1) as rank " +
                "FROM songs " +
                "WHERE song_search_text MATCH 'beach' " +
                "ORDER BY rank DESC";

  db.all(query, function(err, rows) {
    if (err) throw err;

    t.equal(rows[0].id, 'local-4242984372');
    t.equal(rows[0].rank.toFixed(2), '3.97');

    t.equal(rows[5].id, 'local-939853662');
    t.equal(rows[5].rank.toFixed(2), '2.86');
  });
});
