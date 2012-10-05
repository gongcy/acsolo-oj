package com.util;
import java.util.regex.Pattern;

public class Html2Text {
	
 public static String RemoveHtml(String inputString) {   
             String htmlStr = inputString;    
             String textStr ="";   
             java.util.regex.Pattern p_script;   
             java.util.regex.Matcher m_script;   
             java.util.regex.Pattern p_style;   
             java.util.regex.Matcher m_style;   
             java.util.regex.Pattern p_html;   
             java.util.regex.Matcher m_html;   
              
             java.util.regex.Pattern p_html1;   
             java.util.regex.Matcher m_html1;   
           
            try {   
              String regEx_script = "<[\\s]*?script[^>]*?>[\\s\\S]*?<[\\s]*?\\/[\\s]*?script[\\s]*?>"; //����script��������ʽ{��<script[^>]*?>[\\s\\S]*?<\\/script> }   
              String regEx_style = "<[\\s]*?style[^>]*?>[\\s\\S]*?<[\\s]*?\\/[\\s]*?style[\\s]*?>"; //����style��������ʽ{��<style[^>]*?>[\\s\\S]*?<\\/style> }   
                 String regEx_html = "<[^>]+>"; //����HTML��ǩ��������ʽ   
                 String regEx_html1 = "<[^>]+";   
                 p_script = Pattern.compile(regEx_script,Pattern.CASE_INSENSITIVE);   
                 m_script = p_script.matcher(htmlStr);   
                 htmlStr = m_script.replaceAll(""); //����script��ǩ   
    
                 p_style = Pattern.compile(regEx_style,Pattern.CASE_INSENSITIVE);   
                 m_style = p_style.matcher(htmlStr);   
                 htmlStr = m_style.replaceAll(""); //����style��ǩ   
              
                 p_html = Pattern.compile(regEx_html,Pattern.CASE_INSENSITIVE);   
                 m_html = p_html.matcher(htmlStr);   
                 htmlStr = m_html.replaceAll(""); //����html��ǩ   
                  
                 p_html1 = Pattern.compile(regEx_html1,Pattern.CASE_INSENSITIVE);   
                 m_html1 = p_html1.matcher(htmlStr);   
                 htmlStr = m_html1.replaceAll(""); //����html��ǩ   
              
                  
              textStr = htmlStr;   
             
          }catch(Exception e) {   
                   System.err.println("Html2Text: " + e.getMessage());   
           }   
          
         return textStr;//�����ı��ַ���   
              }   
  
 public static void main(String[] arg){
  String strtext = "<P><TR>��</TR><a>\"vv\"     v</a></P>";
  System.out.println(RemoveHtml(strtext));
  
 }
}