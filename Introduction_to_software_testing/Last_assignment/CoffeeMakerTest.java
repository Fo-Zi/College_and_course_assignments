
package edu.ncsu.csc326.coffeemaker;

import static org.mockito.Mockito.*;
import static org.junit.Assert.assertEquals;

import org.junit.Before;
import org.junit.Test;

import edu.ncsu.csc326.coffeemaker.exceptions.InventoryException;
import edu.ncsu.csc326.coffeemaker.exceptions.RecipeException;

/**
 * Unit tests for CoffeeMaker class.
 * 
 * @author Zingarelli Facundo
 */
public class CoffeeMakerTest {
	
	/**
	 * The object under test.
	 */
	private CoffeeMaker coffeeMaker;
	
	// Sample recipes to use in testing.
	private Recipe recipe1;
	private Recipe recipe2;
	private Recipe recipe3;
	private Recipe recipe4;
	private Recipe recipe5;
	private Recipe recipe6;
	private Recipe recipe7;

	private Recipe [] stubRecipies; 
	private RecipeBook recipeBookStub;
	/**
	 * Initializes some recipes to test with and the {@link CoffeeMaker} 
	 * object we wish to test.
	 * 
	 * @throws RecipeException  if there was an error parsing the ingredient 
	 * 		amount when setting up the recipe.
	 */
	@Before
	public void setUp() throws RecipeException {

		recipeBookStub = mock(RecipeBook.class);
		coffeeMaker = new CoffeeMaker(recipeBookStub, new Inventory());
		
		//Set up for r1
		recipe1 = new Recipe();
		recipe1.setName("Coffee");
		recipe1.setAmtChocolate("0");
		recipe1.setAmtCoffee("3");
		recipe1.setAmtMilk("1");
		recipe1.setAmtSugar("1");
		recipe1.setPrice("50");
		
		//Set up for r2
		recipe2 = new Recipe();
		recipe2.setName("Mocha");
		recipe2.setAmtChocolate("20");
		recipe2.setAmtCoffee("3");
		recipe2.setAmtMilk("1");
		recipe2.setAmtSugar("1");
		recipe2.setPrice("75");
		
		//Set up for r3
		recipe3 = new Recipe();
		recipe3.setName("Latte");
		recipe3.setAmtChocolate("0");
		recipe3.setAmtCoffee("3");
		recipe3.setAmtMilk("3");
		recipe3.setAmtSugar("1");
		recipe3.setPrice("100");
		
		//Set up for r4
		recipe4 = new Recipe();
		recipe4.setName("Hot Chocolate");
		recipe4.setAmtChocolate("4");
		recipe4.setAmtCoffee("0");
		recipe4.setAmtMilk("1");
		recipe4.setAmtSugar("1");
		recipe4.setPrice("65");
		
		//Set up for r4
		recipe5 = new Recipe();
		recipe5.setName("Custom 1");
		recipe5.setAmtChocolate("1");
		recipe5.setAmtCoffee("50");
		recipe5.setAmtMilk("1");
		recipe5.setAmtSugar("1");
		recipe5.setPrice("65");		
		
		//Set up for r4
		recipe6 = new Recipe();
		recipe6.setName("Custom 2");
		recipe6.setAmtChocolate("1");
		recipe6.setAmtCoffee("1");
		recipe6.setAmtMilk("50");
		recipe6.setAmtSugar("1");
		recipe6.setPrice("65");	
		
		//Set up for r4
		recipe7 = new Recipe();
		recipe7.setName("Custom 3");
		recipe7.setAmtChocolate("1");
		recipe7.setAmtCoffee("1");
		recipe7.setAmtMilk("1");
		recipe7.setAmtSugar("50");
		recipe7.setPrice("65");	
		
	}
	
	
	/**
	 * Given a coffee maker with the default inventory
	 * When we add inventory with well-formed quantities
	 * Then we do not get an exception trying to read the inventory quantities.
	 * 
	 * @throws InventoryException  if there was an error parsing the quanity
	 * 		to a positive integer.
	 */
	@Test
	public void testAddInventory() throws InventoryException {
		coffeeMaker.addInventory("100","100","100","100");
	}
	
	/**
	 * Given a Coffee Maker, we check that the expected default
	 * inventory is set.
	 * @throws InventoryException  if there was an error parsing the quanity
	 * 		to a positive integer.
	 */
	@Test
	public void testDefaultInventory() throws InventoryException {
		String expected_output = "Coffee: 15\nMilk: 15\nSugar: 15\nChocolate: 15\n" ;
		assertEquals(expected_output, coffeeMaker.checkInventory());
	}
	
	/**
	 * Given a coffee maker with the default inventory
	 * When we add inventory with malformed quantities (i.e., a negative 
	 * quantity and a non-numeric string)
	 * Then we get an inventory exception
	 * 
	 * @throws InventoryException  if there was an error parsing the quanity
	 * 		to a positive integer.
	 */
	@Test(expected = InventoryException.class)
	public void testAddInventoryException1() throws InventoryException {
		coffeeMaker.addInventory("4", "1", "2", "-3");
		
	}
	@Test(expected = InventoryException.class)
	public void testAddInventoryException2() throws InventoryException {
		coffeeMaker.addInventory("4", "1", "-2", "23");
		
	}
	@Test(expected = InventoryException.class)
	public void testAddInventoryException3() throws InventoryException {
		coffeeMaker.addInventory("4", "-1", "02", "13");
		
	}
	
	@Test(expected = InventoryException.class)
	public void testAddInventoryException4() throws InventoryException {
		coffeeMaker.addInventory("-4", "01", "12", "33");
		
	}
	
	@Test(expected = InventoryException.class)
	public void testAddInventoryException5() throws InventoryException {
		coffeeMaker.addInventory("!%Z", "1", "2", "3");
		
	}
	
	@Test(expected = InventoryException.class)
	public void testAddInventoryException6() throws InventoryException {
		coffeeMaker.addInventory("47", "%!a", "2", "3");
		
	}
	
	@Test(expected = InventoryException.class)
	public void testAddInventoryException7() throws InventoryException {
		coffeeMaker.addInventory("47", "1", "%!#%", "3");
		
	}
	
	@Test(expected = InventoryException.class)
	public void testAddInventoryException8() throws InventoryException {
		coffeeMaker.addInventory("47", "12", "2", "*^!@");
		
	}
	/**
	 * Given a coffee maker with the default inventory,
	 * we check that the added amount it's correctly updated.
	 * 
	 * @throws InventoryException  if there was an error parsing the quantity
	 * 		to a positive integer.
	 */
	@Test
	public void testCheckInventory() throws InventoryException {
		String expected_output = "Coffee: 19\nMilk: 16\nSugar: 1015\nChocolate: 18\n" ;
		coffeeMaker.addInventory("4", "1", "1000", "3");
		assertEquals(expected_output, coffeeMaker.checkInventory());
	}
	
	/**
	 * Given a coffee maker, we check that a suited input
	 * is actually added.
	 * 
	 * @throws InventoryException  if there was an error parsing the quanity
	 * 		to a positive integer.
	 */
	
	
	/**
	 * Given a coffee maker, we check that a suited input
	 * is actually added.
	 * 
	 * @throws InventoryException  if there was an error parsing the quanity
	 * 		to a positive integer.
	 */
	
	@Test
	public void testAddRecipeTrue(){
		assertEquals(true, coffeeMaker.addRecipe(recipe2));
	}
	
	/**
	 * Given a coffee maker, we test that multiple suited recipes
	 * are correctly added. 
	 * 
	 * @throws InventoryException  if there was an error parsing the quanity
	 * 		to a positive integer.
	 */
	
	@Test
	public void testAddMultipleRecipesTrue(){
		coffeeMaker.addRecipe(recipe1);
		assertEquals(true, coffeeMaker.addRecipe(recipe2));
		assertEquals(true, coffeeMaker.addRecipe(recipe3));
	}
	
	/**
	 * Given a coffee maker, we check that when we try to
	 * add an excessive amount of recipes, it fails.
	 * 
	 * @throws InventoryException  if there was an error parsing the quanity
	 * 		to a positive integer.
	 */
	
	@Test
	public void testAddRecipeTooManyRecipes(){
		coffeeMaker.addRecipe(recipe1);
		coffeeMaker.addRecipe(recipe2);
		coffeeMaker.addRecipe(recipe3);
		assertEquals(false, coffeeMaker.addRecipe(recipe4));
	}
	
	/**
	 * Given a coffee maker, we check that we cannot
	 * add the same recipe more than once.
	 * 
	 * @throws InventoryException  if there was an error parsing the quanity
	 * 		to a positive integer.
	 */
	@Test
	public void testAddRepeatedRecipe(){
		coffeeMaker.addRecipe(recipe1);
		assertEquals(false, coffeeMaker.addRecipe(recipe1));
		assertEquals(false, coffeeMaker.addRecipe(recipe1));
	}
	
	/**
	 * Given a coffee maker, we check that after editing
	 * a recipe, it's attributes are correctly updated
	 * 
	 * @throws InventoryException  if there was an error parsing the quanity
	 * 		to a positive integer.
	 */
	@Test
	public void testEditRecipeExpectedAttributes(){
		coffeeMaker.addRecipe(recipe3);
		coffeeMaker.addRecipe(recipe1);
		coffeeMaker.editRecipe(0, recipe2);
		
		String expected_name = "Latte";
		int expected_AmtChocolate = 20;
		int expected_AmtCoffee = 3;
		int expected_AmtMilk = 1;
		int expected_AmtSugar = 1;
		int expected_Price = 75;
		
		assertEquals(expected_name,coffeeMaker.getRecipes()[0].getName() );
		assertEquals(expected_AmtChocolate,coffeeMaker.getRecipes()[0].getAmtChocolate() );
		assertEquals(expected_AmtCoffee,coffeeMaker.getRecipes()[0].getAmtCoffee() );
		assertEquals(expected_AmtMilk,coffeeMaker.getRecipes()[0].getAmtMilk() );
		assertEquals(expected_AmtSugar,coffeeMaker.getRecipes()[0].getAmtSugar() );
		assertEquals(expected_Price,coffeeMaker.getRecipes()[0].getPrice() );
	}
	
	/**
	 * Given a coffee maker, we check that when we try to edit
	 * an non-existing recipe, it returns null.
	 * 
	 * @throws InventoryException  if there was an error parsing the quanity
	 * 		to a positive integer.
	 */
	@Test
	public void testEditNullRecipe(){
		assertEquals(null, coffeeMaker.editRecipe(0, recipe3));
	}
	
	/**
	 * Given a coffee maker, we check that when we try to edit
	 * a recipe with an incorrect index, it returns null.
	 * 
	 * @throws InventoryException  if there was an error parsing the quanity
	 * 		to a positive integer.
	 */
	@Test
	public void testEditNegativeRecipe(){
		assertEquals(null, coffeeMaker.editRecipe(-4, recipe3));
	}
	
	/**
	 * Given a coffee maker, we check that we we delete a recipe, it
	 * returns the name of the deleted recipe.
	 * 
	 * @throws InventoryException  if there was an error parsing the quanity
	 * 		to a positive integer.
	 */
	
	@Test
	public void testDeleteRecipe1(){
		coffeeMaker.addRecipe(recipe1);
		coffeeMaker.addRecipe(recipe2);
		coffeeMaker.addRecipe(recipe3);
		String expected_output = "Mocha";
		assertEquals(expected_output, coffeeMaker.deleteRecipe(1));
	}
	
	/**
	 * Given a coffee maker, we check that when we try to access
	 * the attribute of a supposedly deleted recipe, we get a null pointer
	 * exception.
	 * 
	 * @throws InventoryException  if there was an error parsing the quanity
	 * 		to a positive integer.
	 */
	
	@Test(expected = NullPointerException.class)
	public void testDeleteRecipe2(){
		coffeeMaker.addRecipe(recipe1);
		coffeeMaker.addRecipe(recipe2);
		coffeeMaker.addRecipe(recipe3);
		coffeeMaker.deleteRecipe(1);
		Recipe recipes[] = coffeeMaker.getRecipes();
		recipes[1].getName();
	}
	
	@Test
	public void testDeleteWrongRecipe(){
		assertEquals(null, coffeeMaker.deleteRecipe(-1));
		assertEquals(null, coffeeMaker.deleteRecipe(2));
	}
	
	/**
	 * Given a coffee maker, we check that the correct amount of
	 * payment is processed.
	 * 
	 */
	@Test
	public void testMakeCoffeeEvenPayment() {
		coffeeMaker.addRecipe(recipe1);
		assertEquals(0, coffeeMaker.makeCoffee(0, 50));
	}
	
	/**
	 * Given a coffee maker, we check that it returns the input amount
	 * when it's negative
	 * 
	 * @throws InventoryException  if there was an error parsing the quanity
	 * 		to a positive integer.
	 */
	@Test
	public void testMakeCoffeeNegativePayment() {
		coffeeMaker.addRecipe(recipe1);
		assertEquals(-50, coffeeMaker.makeCoffee(0, -50));
	}

	/**
	 * Given a coffee maker, we check that when we input more money than
	 * needed, we get the expected change.
	 * 
	 * @throws InventoryException  if there was an error parsing the quanity
	 * 		to a positive integer.
	 */
	@Test
	public void testMakeCoffeeExceedPayment() {
		
		try {
			coffeeMaker.addInventory("100","100","100","100");
		}catch (InventoryException e){
			System.out.println("Not able to fill inventory with test purposes.");
		}
		
		coffeeMaker.addRecipe(recipe1);
		coffeeMaker.addRecipe(recipe2);
		assertEquals(125, coffeeMaker.makeCoffee(1, 200));
	}
	
	/**
	 * Given a coffee maker, we check that when we try to pay with not
	 * enough money, the same amount is returned.
	 * 
	 * @throws InventoryException  if there was an error parsing the quanity
	 * 		to a positive integer.
	 */
	@Test
	public void testMakeCoffeeNotEnoughMoney() {
		
		try {
			coffeeMaker.addInventory("100","100","100","100");
		}catch (InventoryException e){
			System.out.println("Not able to fill inventory with test purposes.");
		}
		
		coffeeMaker.addRecipe(recipe1);
		coffeeMaker.addRecipe(recipe2);
		assertEquals(20, coffeeMaker.makeCoffee(1, 20));
	}
	/**
	 * Given a coffee maker, we check that when we try to pay with not
	 * enough money, the same amount is returned.
	 * 
	 * @throws InventoryException  if there was an error parsing the quanity
	 * 		to a positive integer.
	 */
	
	@Test
	public void testMakeCoffeeNotEnoughIngredients() {
		coffeeMaker.addRecipe(recipe2);
		coffeeMaker.addRecipe(recipe5);
		coffeeMaker.addRecipe(recipe6);
		
		assertEquals(75, coffeeMaker.makeCoffee(0, 75));
		assertEquals(100, coffeeMaker.makeCoffee(1, 100));
		assertEquals(100, coffeeMaker.makeCoffee(2, 100));
		
		coffeeMaker.editRecipe(2,recipe7);
		assertEquals(100, coffeeMaker.makeCoffee(2, 100));

	}
	
	@Test
	public void testMakeCoffeeEmptyRecipe() {
		coffeeMaker.addRecipe(recipe1);
		coffeeMaker.addRecipe(recipe1);
		coffeeMaker.deleteRecipe(0);
		assertEquals(75, coffeeMaker.makeCoffee(0, 75));
	}
	
}
