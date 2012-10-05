package com.gdoj.common.interceptor;

import javax.servlet.http.HttpServletRequest;

import org.apache.struts2.ServletActionContext;

import com.opensymphony.xwork2.ActionContext;
import com.opensymphony.xwork2.ActionInvocation;
import com.opensymphony.xwork2.ActionSupport;
import com.opensymphony.xwork2.interceptor.AbstractInterceptor;

public class LoginInterceptor extends AbstractInterceptor {
	private static final long serialVersionUID = -5718204L;

	public String intercept(ActionInvocation invocation) throws Exception {

		ActionContext actionContext = invocation.getInvocationContext();
		String username = (String)actionContext.getSession().get("session_username");
		//System.out.println("----"+ServletActionContext.getRequest().getRequestURL()); 
		
		// ��ȡHttpServletRequest����         
		//HttpServletRequest req = ServletActionContext.getRequest();      
		// ��ȡ������ĵ�ַ�������ַ����application name������subString������ȥ��application name          
		//String path = req.getRequestURI(); 
		//System.out.println(path+"----"+ServletActionContext.getRequest().getRequestURL()); 
		if( null != username ) {
			//actionContext.put("tip","Login");
			return invocation.invoke();
		}
		//actionContext.put("tip", "You have not login yet or already logoff, please login first!");
		return ActionSupport.LOGIN;
	}
}
