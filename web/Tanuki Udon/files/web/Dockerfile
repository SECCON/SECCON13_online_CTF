FROM node:22.9.0-slim
WORKDIR /app

COPY ["package.json", "package-lock.json", "./"]
RUN npm install
COPY . .

USER 404:404
ENV NODE_ENV=production
CMD ["node", "index.js"]
