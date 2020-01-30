FROM node:10

WORKDIR /usr/src/app

COPY package*.json ./
RUN npm install

COPY . .

EXPOSE 8085

CMD ["npm", "run", "start"]