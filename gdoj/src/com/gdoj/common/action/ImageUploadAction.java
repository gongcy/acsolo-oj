package com.gdoj.common.action;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.util.Date;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.struts2.ServletActionContext;
import org.apache.struts2.dispatcher.multipart.MultiPartRequestWrapper;
import org.springframework.stereotype.Controller;
import com.util.ImageUtils;

import com.opensymphony.xwork2.ActionSupport;

@Controller
public class ImageUploadAction extends ActionSupport{
	
	private static final long serialVersionUID = 6624518147834729694L;

	//ͼƬ����
	private File imgFile;
	//ͼƬ���
	private String imgWidth;
	//ͼƬ�߶�
	private String imgHeight;
	//ͼƬ���뷽ʽ
	private String align;
	//ͼƬ����
	private String imgTitle;
	
	public String uploadImage() throws Exception{
		MultiPartRequestWrapper wrapper = (MultiPartRequestWrapper) ServletActionContext.getRequest();
		
		//���ͼƬ����
		String imgName = wrapper.getFileNames("imgFile")[0];
		
		//���ͼƬ��׺��
		String fileExt = imgName.substring(imgName.lastIndexOf(".")).toLowerCase();
		
		//��������ͼƬ����
		String imgN = new Date().getTime() + fileExt;
		
		//ͼƬ�ڷ������ϵľ���·�����༭����û���ṩɾ��ͼƬ���ܣ���·���Ժ�������ں�̨�����ͼƬ�Ĳ���
		String serverPath = ServletActionContext.getRequest().getRealPath("/upload");
		//ҳ������õ�ַ
		String savePath = "/upload/";
		//ʵ��Ӧ���м��ڵ�ַ�Ŀɱ��ԣ��˴�������path���Զ�̬���ɻ�������ļ���ȡ
		
		kEUploadImage(ServletActionContext.getRequest(), ServletActionContext.getResponse(), imgFile, imgTitle, imgWidth, imgHeight, imgN, savePath, serverPath);
		
		return null;
	}
	
	void kEUploadImage(HttpServletRequest request, HttpServletResponse response, File imgFile, String imgTitle, String imgWidth, String imgHeight, String imgName, String savePath, String serverPath) 
			throws FileNotFoundException, IOException{
		
		//��ͼƬд�������
		ImageUtils.uploadToServer(imgFile, serverPath, imgName);
		
		//ҳ�����
		String id = "content";
		String url = savePath + imgName;
		String border = "0";      
		String result ="<mce:script type=\"text/javaScript\">parent.KE.plugin[\"image\"].insert(\""   
		+ id      
		+ "\",\""     
		+ url      
		+ "\",\""     
		+ imgTitle      
		+ "\",\""   
		+ imgWidth      
		+ "\",\""  
		 + imgHeight    
		 + "\",\""  
		+ border + "\""    
		+")";
// --></mce:script>";      
		PrintWriter out = null; 
		out = encodehead(request, response);
		out.write(result);   
		out.close();
	}
	
	PrintWriter encodehead(HttpServletRequest request,HttpServletResponse response){
		try {
			request.setCharacterEncoding("utf-8");   
			response.setContentType("text/html; charset=utf-8");   
			return response.getWriter();
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
			return null;
		} catch (IOException e) {
			e.printStackTrace();
			return null;
		}   
	}
	
	public File getImgFile() {
		return imgFile;
	}
	public void setImgFile(File imgFile) {
		this.imgFile = imgFile;
	}
	public String getImgWidth() {
		return imgWidth;
	}
	public void setImgWidth(String imgWidth) {
		this.imgWidth = imgWidth;
	}
	public String getImgHeight() {
		return imgHeight;
	}
	public void setImgHeight(String imgHeight) {
		this.imgHeight = imgHeight;
	}
	public String getAlign() {
		return align;
	}
	public void setAlign(String align) {
		this.align = align;
	}
	public String getImgTitle() {
		return imgTitle;
	}
	public void setImgTitle(String imgTitle) {
		this.imgTitle = imgTitle;
	}
}