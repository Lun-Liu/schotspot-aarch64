class WWC1 {
	int x = 0;
	int y = 0;
	int z = 0;
	int tmpx1 = 0;
	int tmpy2 = 0;

	class Thread1 extends Thread {
		public void run(){
			x = 2;
		}
	}

	class Thread2 extends Thread {
		public void run(){
			tmpx1 = x;
			y = 1;
		}
	}

	class Thread3 extends Thread {
		public void run(){
			tmpy2 = y;
			x = 1;
		}
	}


	public void test(){
		Thread1 t1 = new Thread1();
		Thread2 t2 = new Thread2();
		Thread3 t3 = new Thread3();

		t1.start();
		t2.start();
		t3.start();

		try {
			t1.join();
			t2.join();
			t3.join();
		} catch(InterruptedException e) {
			System.out.println(e);
		}
	}


	public static void main(String[] args){
		int counter = 1;
		while (counter <= 100000){
			WWC1 wwc = new WWC1();
			wwc.test();
			if(wwc.tmpx1 == 2 && wwc.tmpy2 == 1 && wwc.x == 2) 
				break;
			counter++;
		}

		if(counter == 100001)
			System.out.println("ARM WWC Test PASSED after 100000 iterations");
		else
			System.out.println("ARM WWC Test FAILED: problem occurs on iteration " + counter);
	}
}
