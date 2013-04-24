package com.gdoj.common.action;

import com.gdoj.user.service.UserService;
import com.gdoj.user.vo.User;
import com.opensymphony.xwork2.ActionContext;
import com.opensymphony.xwork2.ActionSupport;
import com.util.Mail;

public class FindPasswordAction extends ActionSupport {

	/**
	 * 
	 */
	private static final long serialVersionUID = 3552864426020089250L;
	private UserService userService; 
	private String username;

	public UserService getUserService() {
		return userService;
	}
	public void setUserService(UserService userService) {
		this.userService = userService;
	}
	public String getUsername() {
		return username;
	}
	public void setUsername(String username) {
		this.username = username;
	}

	public String execute()throws Exception {
		try {
			
			System.out.println(username+" recover password.");
			if(false==userService.isUsernameExist(username)){
				System.out.println(username + " is not exist.");
				this.addFieldError("username", "username is not exist.");
				return INPUT;
			}
			User user_ = new User();
			user_ = userService.queryUser(username);
			if (null != user_) {
				 Mail sendmail = new Mail();
			     sendmail.setHost("smtp.qq.com");            //发邮件服务器
			     sendmail.setUserName("269574524");         //用户名
			     sendmail.setPassWord("password");           //密码
			     sendmail.setTo(user_.getEmail());            //发送到:sky_zd@126.com
			     sendmail.setFrom("269574524@qq.com");     //发送邮箱
			     sendmail.setSubject("Your Password on Online Judge");             //标题
			     
			     String content_ = new String();
			     content_ = "Hi! "+username+" , Your password is:"+user_.getPassword()+"\n http://acm.guetonline.com";
			     
			     sendmail.setContent(content_);          //邮件内容
			     sendmail.sendMail();
			     ActionContext.getContext().put("tip", "Your password has been send to your Register-Email:"+user_.getEmail()+",please check it!");
				return SUCCESS;
			}else{
				return ERROR;
			}
		} catch (Exception e) {
			// TODO: handle exception
			
			return ERROR;
		}
	}
}
