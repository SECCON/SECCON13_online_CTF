import puppeteer from "puppeteer";

const FLAG = process.env.FLAG ?? console.log("No flag") ?? process.exit(1);

const APP_HOST = "web";
const APP_PORT = "3000";
export const APP_URL = `http://${APP_HOST}:${APP_PORT}`;

// Flag format
if (!/^SECCON{[\x20-\x7f]+}$/.test(FLAG)) {
  console.log("Bad flag");
  process.exit(1);
}

const sleep = async (ms) => new Promise((resolve) => setTimeout(resolve, ms));

export const visit = async (url) => {
  console.log(`start: ${url}`);

  const browser = await puppeteer.launch({
    headless: "new",
    executablePath: "/usr/bin/chromium",
    args: [
      "--no-sandbox",
      "--disable-dev-shm-usage",
      "--disable-gpu",
      '--js-flags="--noexpose_wasm"',
    ],
  });

  const context = await browser.createBrowserContext();

  try {
    // Create a flag note
    const page1 = await context.newPage();
    await page1.goto(APP_URL, { waitUntil: 'networkidle0' });
    await page1.waitForSelector("#noteInput");
    await page1.type("#noteInput", FLAG);
    await page1.waitForSelector("#createNote");
    await page1.click("#createNote");
    await sleep(1 * 1000);
    await page1.close();

    // Visit the given URL
    const page2 = await context.newPage();
    await page2.goto(url, { timeout: 3000 });
    await sleep(10 * 1000);
    await page2.close();
  } catch (e) {
    console.error(e);
  }

  await context.close();
  await browser.close();

  console.log(`end: ${url}`);
};
