const crypto = require('node:crypto');

class Note {
  #locals;

  constructor({ title, content }) {
    const id = crypto.randomBytes(8).toString('hex');
    this.#locals = {
      id,
      title,
      content
    }
  }
  get id() {
    return this.#locals.id;
  }
  get title() {
    return this.#locals.title;
  }
  get content() {
    return this.#locals.content;
  }
}

class User {
  #locals;

  constructor() {
    const id = crypto.randomBytes(8).toString('hex');
    const notes = [];
    this.#locals = {
      id,
      notes,
    };
  }
  get id() {
    return this.#locals.id;
  }
  addNote(note) {
    this.#locals.notes.push(note);
  }
  getNotes() {
    return this.#locals.notes;
  }
}

class Db {
  #users;
  #notes;

  constructor() {
    this.#users = new Map();
    this.#notes = new Map();
  }

  createUser() {
    const user = new User();
    this.#users.set(user.id, user);
    return user;
  }
  getUser(id) {
    return this.#users.get(id);
  }
  deleteUser(id) {
    const user = this.getUser(id);
    user.getNotes().map(note => this.deleteNote(note.id));
    this.#users.delete(id);
  }
  
  createNote({ title, content }) {
    const note = new Note({ title, content });
    this.#notes.set(note.id, note);
    return note;
  }
  getNote(id) {
    return this.#notes.get(id);
  }
  deleteNote(id) {
    this.#notes.delete(id);
  }
}

module.exports = new Db();
