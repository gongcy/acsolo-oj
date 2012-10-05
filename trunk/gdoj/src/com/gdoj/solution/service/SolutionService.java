package com.gdoj.solution.service;

import java.util.List;

import com.gdoj.solution.vo.Solution;

public interface SolutionService {
	public Solution querySolution(Integer solutionId);
	public List<Solution> querySolutions(Integer from,Integer pageSize,String sql);
	public Integer countSolutions(String sql);
	public void save(Solution solution);
	public List<Object> query(String sql);
	
	public List<Integer> getBydaily();
	public List<Integer> getBymonthly();
	
}
