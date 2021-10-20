const express = require('express');
const cors = require('cors')
const {graphqlHTTP} = require('express-graphql');
const { graphql, buildSchema } = require('graphql');
const db={
	history : [
 	 {id: "0", title: '1',duration: 100},
	 {id: '1', title: '2',duration: 100},
 	 {id: '2', title: '3',duration: 100},
	 {id: '3', title: '4',duration: 100},
	 {id: '4', title: '5',duration: 100}
	]
};

//쿼리라는 내장 타입 존재
//String! 이라고 하면 널이 아님을 보장
//콤마쓰면 안됌
//뒤에 타입적는거는 첫글자 대문자!
const schema = buildSchema(`
	type Query {
		tasks: [Task!]!
		task(id: ID!) : Task
	}

	type Task{
		id : ID!
		title : String!
		duration : Int!
	}

	type Mutation {
		addTask(title: String, duration: Int): Task
	}
`);

var root = { 
tasks : () => db.history,
task : ({id})=> {
		console.log(id);
		return db.history.find(item=>item.id===id);
		},
addTask: ({title,duration}) => {
		let newTask= {
		id: String(db.history.length),
		title,
		duration 
		};
		db.history.push(newTask);
		return newTask;
	}
};

//비동기적으로 프로미스 사용
graphql(schema, '{ tasks {id title duration} }', root)
.then((response) => {
  console.dir(response,{depth:null});
}).catch(e=>console.log(e));

//netstat -na | grep -i listen i
//이라고 해보면 4000번 포틀가보일것
let app = express();
app.use(cors({
origin:'*'}));

app.use('/graphql', graphqlHTTP({
  schema: schema,
  rootValue: root,
  graphiql: true
}));

app.listen(4000,()=>console.log("now browse to localhost:4000/graphql"));

