package com.util;


import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * ͼƬ��������
 * @author wusq
 * @since 2011-05-05
 */
public class ImageUtils {
	
	/**
	 * ͼƬ�ϴ����������ķ���
	 * @param upload ͼƬ�ļ�
	 * @param serverPath �����ڷ�������·��
	 * @param imgName ͼƬ����
	 * @since 2011-05-05
	 */
	public static void uploadToServer(File upload, String serverPath, String imgName) throws FileNotFoundException, IOException{      
			File dirPath = new File(serverPath);
			if(!dirPath.exists()){
				dirPath.mkdirs();
			}
			String path = dirPath + "//" + imgName;
			FileOutputStream fos = new FileOutputStream(path);      
			FileInputStream fis = new FileInputStream(upload);      
			byte[] buffer = new byte[1024];      
			int len = 0;      
			while ((len = fis.read(buffer)) > 0) {      
				fos.write(buffer, 0, len);      
			}      
			fos.close();      
			fis.close();
	}
}
