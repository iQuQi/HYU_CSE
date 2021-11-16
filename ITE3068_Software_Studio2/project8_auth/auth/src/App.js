import logo from './logo.svg';
import {Component} from "react";
import './App.css';
import {withAuthenticator, AmplifySignOut} from '@aws-amplify/ui-react';
import {Storage} from 'aws-amplify';
import {v4 as uuid} from 'uuid';

class App extends Component{
  constructor(){
    super();
    this.state={images:[]};
  }

  componentDidMount = () => this.fetchImages();

  fetchImages = () => {
    Storage.list("")
    .then(objs => Promise.all(objs.map(obj=>Storage.get(obj.key))))
    .then(images => this.setState({images}))
    .catch(e=>console.log('error : ' + e));
  }

  onChange=e=>{
    const file = e.target.files[0];
    const filetype = file.name.split(".").pop();

    Storage.put(`${uuid()}.${filetype}`,file)
    .then(()=>this.fetchImages())
    .catch(e=>console.log('onChange error: '+e));
  }

  render(){
    const{images} = this.state;
    return <div className = 'App'>
      <header className = 'App-header'>
        <input type="file" onChange={this.onChange}/>
        {images.map(image => <img src = {image} key={image} style={{width: 500}}/>)}
        </header>
      </div>
  }

}
export default withAuthenticator(App);
