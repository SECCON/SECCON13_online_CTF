FROM node:22.9.0-slim

EXPOSE 8080

WORKDIR /app

COPY ["package.json", "package-lock.json", "./"]
RUN npm install

COPY . .

CMD ["node", "index.js"]
