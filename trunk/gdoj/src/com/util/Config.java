/**
 * 
 */
package com.util;

import java.io.InputStream;
import java.util.Properties;

import org.apache.log4j.Logger;

public class Config {

	private final static Logger log = Logger.getLogger(Config.class);
	private static final String PATH = "/config.properties";
	private static Properties properties = null;
	
	public static String getValue(String key) {
		/**
		 * get value matches the key.
		 */
		load();
		
		return properties.getProperty(key);
	}
	
	private static void load() {
		properties = new Properties();
		try {
			InputStream is = Config.class.getResourceAsStream(PATH);
			properties.load(is);
		} catch (Exception e) {
			e.printStackTrace();
			log.error("can not load config.properties");
		}
	}
}
