const aws=require("aws-sdk");
const sharp=require("sharp");

const s3=new aws.S3();

// eslint-disable-next-line
exports.handler = async function (event) {
  console.log('Received S3 event:', JSON.stringify(event, null, 2));
  // Get the object from the event and show its content type
  const Bucket = event.Records[0].s3.bucket.name; //eslint-disable-line
  const Key = event.Records[0].s3.object.key; //eslint-disable-line
  console.log(`Bucket: ${bucket}`, `Key: ${key}`);

  try{
  	let Body = await s3.getObject({Bucket,Key}).promise();
	Body = await sharp(Body.Body);
	let metadata= await Body.metadata();

	if(metadata.width>1024){
		Body=await Body.resize({width:1024}).toBuffer();
		await s3.putObject({Bucket, Key,Body}).promise();
	}

	}.catch(e){
		console.log(`Error:${e}`);
	}
  

};
