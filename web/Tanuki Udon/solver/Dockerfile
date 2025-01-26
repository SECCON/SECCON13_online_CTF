FROM node:22.9.0-slim
WORKDIR /app

EXPOSE 8080

COPY ["package.json", "package-lock.json", "./"]
RUN npm install
COPY . .

CMD ["node", "index.js"]
