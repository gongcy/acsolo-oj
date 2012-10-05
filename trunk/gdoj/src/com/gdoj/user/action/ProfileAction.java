package com.gdoj.user.action;

import java.util.ArrayList;
import java.util.List;

import javax.servlet.http.HttpServletRequest;

import org.apache.struts2.ServletActionContext;

import com.gdoj.problem.service.ProblemService;
import com.gdoj.problem.vo.Problem;
import com.gdoj.solution.service.SolutionService;
import com.gdoj.user.service.UserService;
import com.gdoj.user.vo.User;
import com.opensymphony.xwork2.ActionContext;
import com.opensymphony.xwork2.ActionSupport;

public class ProfileAction extends ActionSupport {
	private static final long serialVersionUID = 1L;
	private UserService userService; 
	private SolutionService solutionService; 
	private ProblemService problemService; 
	
	private List<Problem> problemSolvedList;
	private List<Problem> problemTryList;
	
	private User user;
	private String username;
	private Integer rank;
	
	public Integer getRank() {
		return rank;
	}
	public void setRank(Integer rank) {
		this.rank = rank;
	}
	public SolutionService getSolutionService() {
		return solutionService;
	}
	public void setSolutionService(SolutionService solutionService) {
		this.solutionService = solutionService;
	}
	public ProblemService getProblemService() {
		return problemService;
	}
	public void setProblemService(ProblemService problemService) {
		this.problemService = problemService;
	}
	public List<Problem> getProblemSolvedList() {
		return problemSolvedList;
	}
	public void setProblemSolvedList(List<Problem> problemSolvedList) {
		this.problemSolvedList = problemSolvedList;
	}
	public List<Problem> getProblemTryList() {
		return problemTryList;
	}
	public void setProblemTryList(List<Problem> problemTryList) {
		this.problemTryList = problemTryList;
	}
	public String getUsername() {
		return username;
	}
	public void setUsername(String username) {
		this.username = username;
	}
	public User getUser() {
		return user;
	}
	public void setUser(User user) {
		this.user = user;
	}
	public UserService getUserService() {
		return userService;
	}
	public void setUserService(UserService userService) {
		this.userService = userService;
	}

	public String queryUser()  throws Exception {
		try {
			if(username==null){
				username=(String)ActionContext.getContext().getSession().get("session_username");
			}
			if(username==null){
				return LOGIN;
			}
			user = new User();
			user = userService.queryUser(username);
			if(user==null){
				this.addFieldError("tip", "No such user!");
				return ERROR;
			}
			
			rank = userService.getUserRank(user);
			
			String sql = "select DISTINCT s.problem_id from Solution s where s.verdict=5 and s.username='"+user.getUsername()+"' order by s.problem_id ASC;";
			List<Object> solvedProblemIdList = new ArrayList<Object>();
			solvedProblemIdList = solutionService.query(sql);
			problemSolvedList = new ArrayList<Problem>();
			for(Object i:solvedProblemIdList){
			//	System.out.println(i);
				Problem problem_ = new Problem();
				problem_ = problemService.queryProblem((Integer) i);
				if(problem_!=null){
					problemSolvedList.add(problem_);
				}
			}
			
			sql="select distinct s.problem_id from Solution s where "+
			"s.username='"+user.getUsername()+"' group by s.problem_id " +
			"having SUM(s.verdict=5)<1 order by s.problem_id ASC;";
			
			
			List<Object> tryProblemIdList = new ArrayList<Object>();
			tryProblemIdList = solutionService.query(sql);
			problemTryList = new ArrayList<Problem>();
			for(Object i:tryProblemIdList){
				//System.out.println(i);
				Problem problem_ = new Problem();
				problem_ = problemService.queryProblem((Integer) i);
				if(problem_!=null){
					problemTryList.add(problem_);
				}
			}
			
			
		} catch (Exception e) {
			// TODO: handle exception
			return ERROR;
		}
		return SUCCESS;
	}		
}
