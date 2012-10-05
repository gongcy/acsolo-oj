package com.gdoj.common.action;


import java.util.ArrayList;
import java.util.List;

import org.apache.struts2.ServletActionContext;

import com.gdoj.user.service.UserService;
import com.gdoj.user.vo.User;
import com.opensymphony.xwork2.ActionContext;
import com.opensymphony.xwork2.ActionSupport;
import com.util.OnlineUserList;

public class JsonOnlineUserAction extends ActionSupport {

	/**
	 * 
	 */
	private static final long serialVersionUID = 3552864426020089250L;
	
	private boolean success;
	private String error;
	private List<String> usersList;
	
	public List<String> getUsersList() {
		return usersList;
	}
	public void setUsersList(List<String> usersList) {
		this.usersList = usersList;
	}
	public boolean isSuccess() {
		return success;
	}
	public void setSuccess(boolean success) {
		this.success = success;
	}
	public String getError() {
		return error;
	}
	public void setError(String error) {
		this.error = error;
	} 


	public String onlineUsers()throws Exception {
		try {
			//
			usersList = new ArrayList<String>();
			usersList = OnlineUserList.list();
			//System.out.println(usersList.size());
			success = true;
			return SUCCESS;
			
		} catch (Exception e) {
			// TODO: handle exception
			success = false;
			error = "get online users error.";
			return SUCCESS;
		}
	}
}
