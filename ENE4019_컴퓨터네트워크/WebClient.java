package project2;
import java.io.* ; 
import java.io.IOException;
import java.net.* ; 
import java.util.* ;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.image.BufferedImage;

public class WebClient {

	public static void main(String[] args) throws IOException{
		WebClient client=new WebClient();  
		String ht,dt;
		String s;
		int c;

		Scanner sc=new Scanner(System.in);
		
		while(true) {
			System.out.println("Select Command(1: Get 2:Post 3:exit)");
			c=sc.nextInt();
			ht=sc.nextLine();
			System.out.println("Insert URL");
			ht=sc.nextLine();
			if(c==1) {
				s = client.getWebContentByGet(ht);  
				System.out.println(s);
			}
			else if(c==2) {
				System.out.println("Insert data");
				dt=sc.nextLine();
				s = client.getWebContentByPost(ht,dt);  
				System.out.println(s);
			}
			else {
				break;
			}
			
		
		}
		
	}

	public String getWebContentByPost(String urlString,String data) throws IOException {  
		return getWebContentByPost(urlString, data,"UTF-8", 5000);//iso-8859-1  
	}  

	public String getWebContentByGet(String urlString) throws IOException {  
		return getWebContentByGet(urlString, "iso-8859-1", 5000);  
	} 

	public String getWebContentByGet(String urlString, final String charset, int timeout) throws IOException {
		if (urlString == null || urlString.length() == 0) {
			return null;
		}
		urlString = (urlString.startsWith("http://") || urlString
				.startsWith("https://")) ? urlString : ("http://" + urlString).intern();
		URL url = new URL(urlString);
		HttpURLConnection conn = (HttpURLConnection) url.openConnection();
		conn.setRequestMethod("GET");
		conn.setRequestProperty(
						"User-Agent",
						"2019062833/YOUJINKIM/WebClient/ComNet");
		conn.setRequestProperty("Accept", "text/html");
		conn.setConnectTimeout(timeout);
		try {
			if (conn.getResponseCode() != HttpURLConnection.HTTP_OK) {
				return null;
			}
		} catch (IOException e) {
			e.printStackTrace();
			return null;
		}
		InputStream input = conn.getInputStream();
		BufferedReader reader = new BufferedReader(new InputStreamReader(input,charset));
		String line = null;
		StringBuffer sb = new StringBuffer();
		while ((line = reader.readLine()) != null) {
			sb.append(line).append("\r\n");
		}
		
		if(urlString.contains("jpg")) saveImage(url);
		if (reader != null) {
			reader.close();
		}
		if (conn != null) {
			conn.disconnect();
		}
		return sb.toString();
	}

	public String getWebContentByPost(String urlString, String data, final String charset, int timeout) throws IOException{
		if (urlString == null || urlString.length() == 0) {
			return null;
		}
		urlString = (urlString.startsWith("http://") || urlString.startsWith("https://")) ? urlString : ("http://" + urlString).intern();
		URL url = new URL(urlString);
		HttpURLConnection connection = (HttpURLConnection) url.openConnection();
        connection.setDoOutput(true);   
        connection.setDoInput(true); 
        connection.setRequestMethod("POST");
        connection.setUseCaches(false);
        connection.setInstanceFollowRedirects(true);
        connection.setRequestProperty("Content-Type","text/xml;charset=UTF-8");
        connection.setRequestProperty("User-Agent","2019062833/YOUJINKIM/WebClient/ComNet");
        connection.setRequestProperty("Accept", "text/xml");
        connection.setConnectTimeout(timeout);
        connection.connect();
        DataOutputStream out = new DataOutputStream(connection.getOutputStream());
        byte[] content = data.getBytes("UTF-8");
        out.write(content);
        out.flush();   
        out.close();
        
        try {
			if (connection.getResponseCode() != HttpURLConnection.HTTP_OK) {
				return null;
			}
		} catch (IOException e) {
			e.printStackTrace();
			return null;
		}
        BufferedReader reader = new BufferedReader(new InputStreamReader(connection.getInputStream(),charset));
        String line;
        StringBuffer sb=new StringBuffer();
        while ((line = reader.readLine()) != null) {
            sb.append(line).append("\r\n");
        }
        if (reader != null) {
			reader.close();
		}
		if (connection != null) {
			connection.disconnect();
		}
		return sb.toString();
	} 
	
	 private void saveImage(URL url) throws IOException {
	        InputStream in = null;
	        OutputStream out = null;
	        
	     
	        try {
	            in = url.openStream();
	            out = new FileOutputStream("C:\\tmp\\test.jpg"); //저장경로
	 
	            while(true){
	                //이미지를 읽어온다.
	                int data = in.read();
	                if(data == -1){
	                    break;
	                }
	                //이미지를 쓴다.
	                out.write(data);
	            }
	            in.close();
	            out.close();
	        } catch (Exception e) {
	            e.printStackTrace();
	        }finally{
	            if(in != null){in.close();}
	            if(out != null){out.close();}
	        }
	        
	        drawImage();
	    }
	 private void drawImage()throws IOException{
		        File file = new File("C:\\tmp\\test.jpg");
		        BufferedImage img = ImageIO.read(file);
		        JLabel lb = new JLabel(new ImageIcon(img));
		        JFrame f = new JFrame("이미지 그리기");
		        f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		        f.getContentPane().add(lb);
		        f.pack();
		        f.setLocation(200,200);
		        f.setVisible(true);
		    }
		
}





