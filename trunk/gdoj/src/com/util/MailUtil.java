
package com.util;


import java.util.Date;
import java.util.Properties;
import java.util.Vector;

import javax.activation.DataHandler;
import javax.activation.FileDataSource;
import javax.mail.Authenticator;
import javax.mail.Message;
import javax.mail.MessagingException;
import javax.mail.Multipart;
import javax.mail.PasswordAuthentication;
import javax.mail.Session;
import javax.mail.Transport;
import javax.mail.internet.InternetAddress;
import javax.mail.internet.MimeBodyPart;
import javax.mail.internet.MimeMessage;
import javax.mail.internet.MimeMultipart;
import javax.mail.internet.MimeUtility;


/*��JAVA�����ʼ�
 *1. ���أ�jaf-1_1-fr.zip��javamail-1_4.zip  ��ѹ�������ɣ�activation.jar��mail.jar
 *��www.zhoujianboy.ys168.com����www.sun.com��������
 *2. ��Jcreator->Configure->Options->JDK Profiles->���ұ�ѡ��JDK��Ȼ��->edit
 *->Add->Add Archive..->�ֱ���ӣ�activation.jar��mail.jar��������
 *3. �������������ʹ��javax.activation��javax.mail�ˡ�
 * Title: ʹ��javamail�����ʼ�
 * Description: ��ʾ���ʹ��javamail�����͵����ʼ������ʵ���ɷ��Ͷ฽��
 */
public class MailUtil
{
    
    String to = ""; // �ռ���

    String from = ""; // ������
    
    String host = ""; // smtp����
    
    String username = "";
    
    String password = "";
    
    String filename = ""; // �����ļ���
    
    String subject = ""; // �ʼ�����
    
    String content = ""; // �ʼ�����
    
    Vector file = new Vector(); // �����ļ�����
    
    /**
     * <br/>
     * ����˵����Ĭ�Ϲ����� <br/>
     * ��������� <br/>
     * �������ͣ�
     */

	public MailUtil() {
		// TODO Auto-generated constructor stub
	}

	/**
     * <br/>
     * ����˵���������ʼ���������ַ <br/>
     * ���������String host �ʼ���������ַ���� <br/>
     * �������ͣ�
     */
    public void setHost(String host)
    {
        this.host = host;
    }
    
    /**
     * <br/>
     * ����˵�������õ�¼������У������ <br/>
     * ��������� <br/>
     * �������ͣ�
     */
    public void setPassWord(String pwd)
    {
        this.password = pwd;
    }
    
    /**
     * <br/>
     * ����˵�������õ�¼������У���û� <br/>
     * ��������� <br/>
     * �������ͣ�
     */
    public void setUserName(String usn)
    {
        this.username = usn;
    }
    
    /**
     * <br/>
     * ����˵���������ʼ�����Ŀ������ <br/>
     * ��������� <br/>
     * �������ͣ�
     */
    public void setTo(String to)
    {
        this.to = to;
    }
    
    /**
     * <br/>
     * ����˵���������ʼ�����Դ���� <br/>
     * ��������� <br/>
     * �������ͣ�
     */
    public void setFrom(String from)
    {
        this.from = from;
    }
    
    /**
     * <br/>
     * ����˵���������ʼ����� <br/>
     * ��������� <br/>
     * �������ͣ�
     */
    public void setSubject(String subject)
    {
        this.subject = subject;
    }
    
    /**
     * <br/>
     * ����˵���������ʼ����� <br/>
     * ��������� <br/>
     * �������ͣ�
     */
    public void setContent(String content)
    {
        this.content = content;
    }
    
    /**
     * <br/>
     * ����˵����������ת��Ϊ���� <br/>
     * ���������String strText <br/>
     * �������ͣ�
     */
    public String transferChinese(String strText)
    {
        try
        {
            strText = MimeUtility.encodeText(new String(strText.getBytes(),
                    "GB2312"), "GB2312", "B");
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        return strText;
    }
    
    /**
     * <br/>
     * ����˵�����������������Ӹ��� <br/>
     * ��������� <br/>
     * �������ͣ�
     */
    public void attachfile(String fname)
    {
        file.addElement(fname);
    }
    
    /**
     * <br/>
     * ����˵���������ʼ� <br/>
     * ��������� <br/>
     * �������ͣ�boolean �ɹ�Ϊtrue����֮Ϊfalse
     */
    public boolean sendMail()
    {
        
// ����mail session
        Properties props = System.getProperties();
        props.put("mail.smtp.host", host);
        props.put("mail.smtp.auth", "true");
        
        Session session = Session.getDefaultInstance(props,
                new Authenticator()
        {
            public PasswordAuthentication getPasswordAuthentication()
            {
                return new PasswordAuthentication(username, password);
            }
        });
        
        try
        {
// ����MimeMessage ���趨������ֵ
            MimeMessage msg = new MimeMessage(session);
            msg.setFrom(new InternetAddress(from));
           // System.out.println(from);
            
            InternetAddress[] address = { new InternetAddress(to) };
            msg.setRecipients(Message.RecipientType.TO, address);
            subject = transferChinese(subject);
            msg.setSubject(subject);
            
// ����Multipart
            Multipart mp = new MimeMultipart();
            
// ��Multipart�������
            MimeBodyPart mbpContent = new MimeBodyPart();
            mbpContent.setText(content);
// ��MimeMessage��ӣ�Multipart�������ģ�
            mp.addBodyPart(mbpContent);
            
// ��Multipart��Ӹ���
/*
 * Enumeration efile = file.elements(); while
 * (efile.hasMoreElements()) {
 *
 * MimeBodyPart mbpFile = new MimeBodyPart(); filename =
 * efile.nextElement().toString(); FileDataSource fds = new
 * FileDataSource(filename); mbpFile.setDataHandler(new
 * DataHandler(fds)); mbpFile.setFileName(fds.getName());
 * //��MimeMessage��ӣ�Multipart�������� mp.addBodyPart(mbpFile); }
 *
 * file.removeAllElements();
 */
// ��Multipart���MimeMessage
            msg.setContent(mp);
            msg.setSentDate(new Date());
// �����ʼ�
            Transport.send(msg);
            
        }
        catch (MessagingException mex)
        {
            mex.printStackTrace();
            Exception ex = null;
            if ((ex = mex.getNextException()) != null)
            {
                ex.printStackTrace();
            }
            return false;
        }
        return true;
    }
    
    /**
     * <br/>
     * ����˵���������������ڲ��� <br/>
     * ��������� <br/>
     * �������ͣ�
     */
   public static void main(String[] args)
    {
        MailUtil sendmail = new MailUtil();
        sendmail.setHost("smtp.qq.com");            //���ʼ�������
        sendmail.setUserName("269574524");         //�û���
        sendmail.setPassWord("password");           //����
        sendmail.setTo("269574524@qq.com");            //���͵�:sky_zd@126.com
        sendmail.setFrom("269574524@qq.com");     //��������
        
        sendmail.setSubject("����ȡ��");             //����
        String content_ = new String();
	    content_ = "<a href='acm.guetonline.com'>Online Judge</a>\n Username:";
	     
        sendmail.setContent(content_);          //�ʼ�����
        sendmail.sendMail();
    }
} // end
