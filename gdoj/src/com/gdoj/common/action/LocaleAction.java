package com.gdoj.common.action;

import java.util.Locale;

import org.apache.struts2.ServletActionContext;

import com.opensymphony.xwork2.ActionContext;
import com.opensymphony.xwork2.ActionSupport;

public class LocaleAction extends ActionSupport {

	/**
	 * 
	 */
	private static final long serialVersionUID = 3552864426020089250L;

		private String lang;  
		private boolean success;
		
		
	    public boolean isSuccess() {
			return success;
		}
		public void setSuccess(boolean success) {
			this.success = success;
		}
		public String getLang() {  
	        return lang;  
	    }   
	    public void setLang(String lang) {  
	        this.lang = lang;  
	    }        
	    @Override  
	    public String execute() throws Exception {  
	        try {
		        Locale currentLocale = Locale.getDefault();  
		        //1������ҳ�����󣬴�����ͬ��Locale����  
		        if("en".equals(getLang().trim())) {  
		            currentLocale = new Locale("en","US");  
		        }else if("zh".equals(getLang().trim())) {  
		            currentLocale = new Locale("zh","CN");  
		        } else{
		        	success=false;
					 return SUCCESS;
		        }
		        /* 
		         * 2������Action�е�Locale 
		         *    ǰ̨ҳ���Locale�ͺ�̨session�е�Locale��Χ�ǲ�һ���� 
		         *    a)ֻ��ҳ��Locale��ǰҳ����Ϣ��ı䵫�ύ��Locale�ֻ�Ļص�Ĭ�ϵ� 
		         *    b)�ı��˺�̨Locale����ǰ�߳��е�ҳ��Locale������ı䣬��������һ���ύ 
		         *      Actionһͬ�ı䣬���Կ���Ҫˢ��ҳ�����Σ���һ��ֻ���̨Locale���ڶ��� 
		         *      ǰ̨�ͺ�̨ͬʱ�ı� 
		         *       
		         *    Ϊ���������������Ҫǰ̨�ͺ�̨��Localeһ��ı� 
		         */  
		       // System.out.println("Locales:"+lang);
		        ActionContext.getContext().setLocale(currentLocale);  
		        ServletActionContext.getRequest().getSession().setAttribute(  
		                "WW_TRANS_I18N_LOCALE", currentLocale);  
		        
		       
		        
			} catch (Exception e) {
				// TODO: handle exception
				success=false;
				 return SUCCESS;
			}
			success=true;
	        return SUCCESS;  
	    }      
}
