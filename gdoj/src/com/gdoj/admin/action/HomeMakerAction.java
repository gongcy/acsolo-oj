package com.gdoj.admin.action;

import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.gdoj.bean.TopUsersBean;
import com.gdoj.user.service.UserService;
import com.gdoj.user.vo.User;

import org.apache.struts2.ServletActionContext;
import org.apache.struts2.json.annotations.JSON;
import org.quartz.SchedulerContext;

import com.opensymphony.xwork2.ActionSupport;
import com.util.MyApplicationContextUtil;
import com.util.StreamHandler;
import com.util.freemarker.MyFreeMarker;

public class HomeMakerAction extends ActionSupport {
	private static final long serialVersionUID = 1L;
	private String content;
	
	public String getContent() {
		return content;
	}

	public void setContent(String content) {
		this.content = content;
	}

	public String homemaker()throws Exception {
		
		try {
			if(content == null) {
				content = "Welcome To GUET Online Judge";
				try {
					String path = ServletActionContext.getRequest()
							.getSession().getServletContext().getRealPath("/");
					//System.out.println(path+"WEB-INF\\templates\\homepage.html");
					content = StreamHandler.read(path
							+ "WEB-INF\\templates\\homepage.html");
				} catch (Exception e) {
					// TODO: handle exception
				}				
			}
			
			Map map = new HashMap();
			map.put("content", content);		
			String root = ServletActionContext.getRequest().getRealPath("/WEB-INF");
						
			
			MyFreeMarker.generator(root, "homepage.ftl", "homepage.html",
					"content", map);
			
		} catch (Exception e) {
			// TODO: handle exception
			return ERROR;
		}
        return SUCCESS;
	}
   
}
