package com.gdoj.solution.action;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import org.apache.struts2.json.annotations.JSON;

import com.gdoj.bean.StatisticBean;
import com.gdoj.contest.service.ContestService;
import com.gdoj.contest.vo.Contest;
import com.gdoj.problem.service.ProblemService;
import com.gdoj.problem.vo.Problem;
import com.gdoj.solution.service.SolutionService;
import com.gdoj.solution.vo.Solution;
import com.gdoj.user.service.UserService;
import com.gdoj.user.vo.User;
import com.opensymphony.xwork2.ActionSupport;

public class StatisticAction extends ActionSupport{

	/**
	 * 
	 */
	private static final long serialVersionUID = -5701824724377184171L;
	
	private SolutionService solutionService;
	private ProblemService problemService;
	private ContestService contestService;
	private UserService userService;
	private String  type;
	private List<StatisticBean> list;
	private boolean success;
	private String error;
	public String getType() {
		return type;
	}
	public void setType(String type) {
		this.type = type;
	}
	public List<StatisticBean> getList() {
		return list;
	}
	public void setList(List<StatisticBean> list) {
		this.list = list;
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
	public String statistic() throws Exception{
		
		try {
			List<Integer> list_ = new ArrayList<Integer>();
			if ("hourly".equals(type)) {
				list_ = null;
			} else if ("daily".equals(type)) {
				list_ = solutionService.getBydaily();
			} else if ("monthly".equals(type)) {
				list_ = solutionService.getBymonthly();
			} else {
				list_ = solutionService.getBydaily();
			}
			list = new ArrayList<StatisticBean>();
			
			for(int i=0;i < list_.size();i++){
				StatisticBean e = new StatisticBean();
				Date t = new Date();
				e.setX(t.getTime()+i*1000*60*24*30);
				e.setY(list_.get(i));
				list.add(e);
			}
			
		} catch (Exception e) {
			// TODO: handle exception
			success = false;		
			return SUCCESS;
		}
		success = true;		
		return SUCCESS;
	}
	
	
	@JSON(deserialize=false,serialize=false)  
	public UserService getUserService() {
		return userService;
	}
	public void setUserService(UserService userService) {
		this.userService = userService;
	}@JSON(deserialize=false,serialize=false)  
	public ContestService getContestService() {
		return contestService;
	}
	public void setContestService(ContestService contestService) {
		this.contestService = contestService;
	}@JSON(deserialize=false,serialize=false)  
	public SolutionService getSolutionService() {
		return solutionService;
	}
	public void setSolutionService(SolutionService solutionService) {
		this.solutionService = solutionService;
	}@JSON(deserialize=false,serialize=false)  
	public ProblemService getProblemService() {
		return problemService;
	}
	public void setProblemService(ProblemService problemService) {
		this.problemService = problemService;
	}

	

	
}
