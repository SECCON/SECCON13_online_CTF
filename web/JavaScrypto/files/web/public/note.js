const getOrCreateKey = () => {
  if (!localStorage.getItem('key')) {
    const rawKey = CryptoJS.lib.WordArray.random(16);
    localStorage.setItem('key', rawKey.toString(CryptoJS.enc.Base64));
  }
  return localStorage.getItem('key');
}

const encryptNote = ({ plaintext, key }) => {
  const rawKey = CryptoJS.enc.Base64.parse(key);
  const rawIv = CryptoJS.lib.WordArray.random(16);
  const rawSalt = CryptoJS.lib.WordArray.random(16);
  const rawCiphertext = CryptoJS.AES.encrypt(plaintext, rawKey, {
    iv: rawIv, 
    salt: rawSalt, 
  }).ciphertext;
  return {
    iv: rawIv.toString(CryptoJS.enc.Base64), 
    ciphertext: rawCiphertext.toString(CryptoJS.enc.Base64), 
  }
}

const decryptNote = ({ key, iv, ciphertext }) => {
  const rawKey = CryptoJS.enc.Base64.parse(key);
  const rawIv = CryptoJS.enc.Base64.parse(iv);
  const rawPlaintext = CryptoJS.AES.decrypt(ciphertext, rawKey, {
    iv: rawIv, 
  });
  return rawPlaintext.toString(CryptoJS.enc.Latin1);
}

const createNote = async ({ plaintext, key }) => {
  const cipherParams = encryptNote({ plaintext, key });
  const { id } = await fetch('/note', {
    method: 'POST', 
    body: JSON.stringify(cipherParams), 
    headers: {
      'content-type': 'application/json'
    }
  }).then(r => r.json());
  return id;
}

const readNote = async ({ id, key }) => {
  const cipherParams = await fetch(`/note/${id}`).then(r => r.json());
  const { iv, ciphertext } = cipherParams;
  return decryptNote({ key, iv, ciphertext });
}
