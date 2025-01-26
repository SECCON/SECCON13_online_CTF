const crypto = require('node:crypto');
const express = require('express');
const session = require('express-session');
const db = require('./db');
const markdown = require('./markdown');

const PORT = '3000';

const app = express();

app.use(express.urlencoded({ extended: false }));
app.use(session({
  secret: crypto.randomBytes(32).toString('base64'),
  resave: true, 
  saveUninitialized: true, 
}));
app.set('view engine', 'ejs');

app.use((req, res, next) => {
  if (!req.session.userId) {
    req.session.userId = db.createUser().id;
  }
  req.user = db.getUser(req.session.userId);
  next();
})

app.use((req, res, next) => {
  if (typeof req.query.k === 'string' && typeof req.query.v === 'string') {
    // Forbidden :)
    if (req.query.k.toLowerCase().includes('content')) return next();

    res.header(req.query.k, req.query.v);
  }
  next();
});

app.get('/', (req, res) => {
  res.render('index', { notes: req.user.getNotes() });
});

app.get('/clear', (req, res) => {
  db.deleteUser(req.user.id);
  req.session.destroy();
  res.redirect('/');
});

app.get('/note/:noteId', (req, res) => {
  const { noteId } = req.params;
  const note = db.getNote(noteId);
  if (!note) return res.status(400).send('Note not found');
  res.render('note', { note });
});

app.post('/note', (req, res) => {
  const { title, content } = req.body;
  req.user.addNote(db.createNote({ title, content: markdown(content) }));
  res.redirect('/');
});

app.listen(PORT, '0.0.0.0', () => {
  console.log(`Listening on port ${PORT}`);
});
