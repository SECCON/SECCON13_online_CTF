const express = require('express');
const crypto = require('crypto');
const app = express();
const PORT = 3000;

const isUUID = (uuid) => /^[0-9a-f]{8}-[0-9a-f]{4}-[0-5][0-9a-f]{3}-[089ab][0-9a-f]{3}-[0-9a-f]{12}$/i.test(uuid);

const notes = new Map();

app.use(express.static('public'));
app.use(express.json());

app.get('/note/:noteId', (req, res) => {

  if (!isUUID(req.params.noteId)) return res.status(404).send('');

  const note = notes.get(req.params.noteId);
  if (!note) return res.status(404).send('');

  return res.json(note);

});

app.post('/note', (req, res) => {

  const { iv, ciphertext } = req.body;
  if (
    typeof iv !== 'string'
    || typeof ciphertext !== 'string'
  ) {
    return res.status(400).send('');
  }

  const id = crypto.randomUUID();
  notes.set(id, { iv, ciphertext });
  
  return res.json({ id });

});

app.listen(PORT, () => {
  console.log(`Server listening on port ${PORT}`)
});
