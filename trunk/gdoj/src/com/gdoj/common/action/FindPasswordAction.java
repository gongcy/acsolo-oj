package com.gdoj.common.action;

import java.util.Date;

import com.gdoj.user.service.UserService;
import com.gdoj.user.vo.User;
import com.opensymphony.xwork2.ActionContext;
import com.opensymphony.xwork2.ActionSupport;
import com.util.Config;
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
			
			Date dt_prevSubmit = (Date)ActionContext.getContext().getSession().get("session_submit");
			Date dt = new Date(); 
			
			if(dt_prevSubmit!=null){
				//System.out.println(dt.getTime()-dt_prevSubmit.getTime());
				if(dt.getTime()-dt_prevSubmit.getTime()<30000){  //限制30s一次提交
					System.out.println(username+" submit twice at 30 second.");
					ActionContext.getContext().put("tip", "Don't operate twice at 30 second, try it after 30 second.");
					return "success";
				}
			}
			ActionContext.getContext().getSession().put("session_submit", dt);
			
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
			     sendmail.setHost(Config.getValue("MAIL_HOST"));            //发邮件服务器
			     sendmail.setUserName(Config.getValue("MAIL_USERNAME"));         //用户名
			     sendmail.setPassWord(Config.getValue("MAIL_PSW"));           //密码
			     sendmail.setTo(user_.getEmail());            //发送到:sky_zd@126.com
			     sendmail.setFrom(Config.getValue("MAIL_FROM"));     //发送邮箱
			     sendmail.setSubject(Config.getValue("MAIL_TITLE"));             //标题
			     
			     /*
			     System.out.println(Config.getValue("MAIL_HOST") + " "+
			    		 Config.getValue("MAIL_USERNAME") + " "+
			    		 Config.getValue("MAIL_PSW") + " "+
			    		 user_.getEmail() + " "+
			    		 Config.getValue("MAIL_FROM") + " "+
			     		 Config.getValue("MAIL_TITLE"));
			     */
			     
			     String content_ = new String();
			     content_ = "Hi! "+username+" , your password is:"+user_.getPassword()+"\n "+Config.getValue("DOMAIN");
			     
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
