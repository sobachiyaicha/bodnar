const fs = require('node:fs');
const { DatabaseSync } = require('node:sqlite');
const db = new DatabaseSync('db/variant.db');
try { db.exec(fs.readFileSync('db/schema.sql','utf8')); db.exec(fs.readFileSync('db/load.sql','utf8')); console.log('Данные загружены в db/variant.db'); } finally { db.close(); }
