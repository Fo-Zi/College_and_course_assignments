import static org.junit.Assert.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintStream;

/**
 * The class containing your tests for the {@link Demo} class.  Make sure you
 * test all methods in this class (including both 
 * {@link Demo#main(String[])} and 
 * {@link Demo#isTriangle(double, double, double)}).
 */

public class DemoTest {
	
	private InputStream inputBackup;
	private PrintStream outputBackup;
	
	private ByteArrayInputStream testIn;
    private ByteArrayOutputStream testOut;
    
    private void setInput(String data) {
    	testIn = new ByteArrayInputStream(data.getBytes());
        System.setIn(testIn);
    }
    
    private String getOutput() {
        String aux = testOut.toString();
        testOut.reset();
        return aux;
    }
    @Test
    public void TESt_isTriangle_Correct_inputs(){
        //Correct input cases 
        assertTrue( Demo.isTriangle(2,2,3) );
        assertTrue( Demo.isTriangle(11,11,7) );
    }
    
    @Test
    public void TESt_isTriangle_Incorrect_inputs(){
        //Incorrect input cases
        assertFalse( Demo.isTriangle(1,2,3) );
        assertFalse( Demo.isTriangle(11,3,7) );
    }
        
    @Test
    public void TESt_isTriangle_negative_inputs(){    
        //Negative input cases
        assertFalse( Demo.isTriangle(-1,2,3) );
        assertFalse( Demo.isTriangle(1,-2,3) );
        assertFalse( Demo.isTriangle(1,2,-3) );
        assertFalse( Demo.isTriangle(-1,-2,3) );
        assertFalse( Demo.isTriangle(-1,2,-3) );
        assertFalse( Demo.isTriangle(1,-2,-3) );
        assertFalse( Demo.isTriangle(-1,-2,-3) );
    }
    
    @Test
    public void TESt_isTriangle_Zero_inputs(){ 
        //Zero input cases
        assertFalse( Demo.isTriangle(0,2,3) );
        assertFalse( Demo.isTriangle(1,0,3) );
        assertFalse( Demo.isTriangle(1,2,0) );
        assertFalse( Demo.isTriangle(0,0,3) );
        assertFalse( Demo.isTriangle(0,2,0) );
        assertFalse( Demo.isTriangle(1,0,0) );
        assertFalse( Demo.isTriangle(0,0,0) );
    }
    
    
    ByteArrayOutputStream out = new ByteArrayOutputStream();
    String[] args = {};
    
    @Before
    public void SetUp() {
    	this.inputBackup = System.in;
    	this.outputBackup = System.out;
    	this.testOut = new ByteArrayOutputStream();
    	System.setOut(new PrintStream(testOut));
    }
    
    @Test
    public void TEST_main_correct_inputs() {
    	//Aux. constants
    	String expected_output1 = "Enter side 1: \nEnter side 2: \nEnter side 3: \nThis is a triangle.\n";    	
    	String input1 = "2\n2\n3\n";
    	String input2 = "11\n11\n7\n";

    	//Correct input test 2 
    	setInput(input1);
    	Demo.main(new String[0]);	
    	assertEquals(expected_output1,getOutput());
    	
    	//Correct input test 2 
    	setInput(input2);
    	Demo.main(new String[0]);
    	assertEquals(expected_output1,getOutput());

    }
    
    @Test
    public void TEST_main_negative_inputs() {
    	//Aux. constants
    	String expected_output1 = "Enter side 1: \nEnter side 2: \nEnter side 3: \nThis is not a triangle.\n";    	
    	String input1 = "-1\n2\n3\n";
    	String input2 = "11\n3\n-7\n";

    	//Correct input test 2 
    	setInput(input1);
    	Demo.main(new String[0]);	
    	assertEquals(expected_output1,getOutput());
    	
    	//Correct input test 2 
    	setInput(input2);
    	Demo.main(new String[0]);
    	assertEquals(expected_output1,getOutput());
    	
    }
    @Test
    public void TEST_main_zero_inputs() {
    	//Aux. constants
    	String expected_output1 = "Enter side 1: \nEnter side 2: \nEnter side 3: \nThis is not a triangle.\n";    	
    	String input1 = "0\n2\n3\n";
    	String input2 = "11\n3\n0\n";

    	//Correct input test 2 
    	setInput(input1);
    	Demo.main(new String[0]);	
    	assertEquals(expected_output1,getOutput());
    	
    	//Correct input test 2 
    	setInput(input2);
    	Demo.main(new String[0]);
    	assertEquals(expected_output1,getOutput());
    	
    }
    
    @Test
    public void TEST_main_incorrect_inputs() {
    	//Aux. constants
    	String expected_output1 = "Enter side 1: \nEnter side 2: \nEnter side 3: \nThis is not a triangle.\n";    	
    	String input1 = "1\n2\n3\n";
    	String input2 = "11\n3\n7\n";

    	//Correct input test 2 
    	setInput(input1);
    	Demo.main(new String[0]);	
    	assertEquals(expected_output1,getOutput());
    	
    	//Correct input test 2 
    	setInput(input2);
    	Demo.main(new String[0]);
    	assertEquals(expected_output1,getOutput());
    	
    }
    
    
    @After
    public void Cleaning() {
    	System.setIn(this.inputBackup);
    	System.setOut(this.outputBackup);
    }
}

