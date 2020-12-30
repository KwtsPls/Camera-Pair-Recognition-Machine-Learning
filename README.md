# Project στο μάθημα Ανάπτυξη Λογισμικού για Πληροφοριακά Συστήματα !!
# Mέλη της Ομάδας :
# 1. Κωνσταντίνος Πλας (1115201700125)
# 2. Ελευθέριος Στέτσικας (1115201700150)  
# Περιγραφή των δομών
## BagOfWords.h
### indexedWord
Είναι μία απλή δομή η οποία περιέχει `char *word`, η οποία είναι η λέξη, `int index`, ο δείκτης της λέξης στο Bag Of Words, και τα `double idf, double tf, double tf_idf_mean` που είναι το tf_idf score της λέξης.
### Functions
#### `indexedWord *createIndexedWord(char *word, int index)`
Αποθηκεύει μία λέξη στο BOW και αρχικοποιεί το tf-idf score της.
#### `void deleteIndexedWord(indexedWord *iw)`
Απελευθερώνει τη μνήμη που χρειάστηκε η indexedWord.
#### `double *getBagOfWords(HashTable *ht,secTable *vocabulary,char *spec_id,char *mode)`
Η συνάρτηση αυτή λαμβάνει το spec_id και ανάλογα το mode επιστρέφει τα tf_idf values σε πίνακα ή τις απλές bag of words τιμές σε πίνακα
`tf-idf` : Με τη χρήση της `findSpecHashTable` λαμβάνουμε το `dictionary` του .json αρχείου του συγκεκριμένου spec και έχοντας αποθηκευμένο το vocabulary μας σε μια δομή secTable, μετράμε πόσες φορές εμφανίζεται η indexedWord στο κείμενο, και δίνουμε και το idf που έχει ήδη αποθηκευτεί για αυτή τη λέξη στο σημείο που είναι indexed. Στο τέλος υπολογίζεται το tf-idf για κάθε indexed λέξη του πίνακα και επιστρέφεται ο πίνακας των τιμών.
`bow` : Παρόμοια με την παραπάνω απλά υπολογίζεται μόνο πόσες φορές εμφανίζεται η συγκεκριμένη λέξη στο κείμενο. 
## Dictionary.h
### Dictionary_node
Είναι μία δομή κόμβου λίστας η οποία χρησιμοποιείται για την αποθήκευση των .json αρχείων στην μνήμη. Γενικά ως `char *key` έχει δηλωθεί κάποιο από τα key του json αρχείου, και στο `char **value` αποθηκεύονται όλα τα values του key. Κρατάμε ακόμα έναν ακέραιο για να γνωρίζουμε πόσα διαφορετικά values μπορεί να έχει κάποιο **key**, `int values_num`. Και ακόμα υπάρχει ένας pointer `struct dict_node *next` που δείχνει στο επόμενο **Dictionary_node** για να αποθηκευτούν και τα υπόλοιπα **key** και **values** του json αρχείου. 
### Dictionary
Eίναι μία δομή στην οποία αποθηκεύουμε `char *dict_namε` το spec_id ενός αρχείου xxxx.json, π.χ. www.ebay.com//xxxx ,όπου αντιστοιχεί στον αριθμό του αρχείου xxxx.json, και ένα δείκτη σε μία λίστα `Dictionary_node *lista`για να αποθηκεύσουμε τα δεδομένα του αρχείου json.
### Functions
#### `Dictionary *initDictionary(char *dict_name)`
Η συνάρτηση αυτή δημιουργεί ένα αντικείμενο dictionary. Αρχικά, κάνει allocate στην μνήμη για το αντικείμενο. Έπειτα αντιγράφει το spec_id στο `Dictionary->dict_name`, χωρίς να αρχικοποιεί τη λίστα για το json αρχείο. Τέλος επιστρέφει το Dictionary που δημιούργησε.
#### `Dictionary *insertDictionary(Dictionary *dic, char *key, char **val, int num_val)`
Η συνάρτηση αυτή χρησιμοποιείται για να εισάγει ένα ζευγάρι key-value από το .json στο Dictionary. Αρχικά, ελέγχει αν το key υπάρχει ήδη μέσα στο Dictionary, αν υπάρχει απελευθερώνει τη μνήμη που είχε κατοχυρωθεί για τα val που ήταν να μπουν και επιστρέφει το παλιό **Dictionary**. Αλλιώς, δημιουργεί ένα καινούργιο κόμβο λίστας **Dictionary_node**, όπου δίνει τις αντίστοιχες τιμές που δέχεται η συνάρτηση ως παράμετρους στις αντίστοιχες μεταβλητές ενός **Dictionary_node**, τέλος ο `Dictionary_node->next` του καινούργιου κόμβου, δείχνει στο `dic->lista` και ο δείκτης λίστας του Dictionary dic δείχνει στον καινούργιο κόμβο, με άλλα λόγια ο καινούργιος κόμβος προστίθεται στην αρχή της λίστας του **dic**.
#### `int updateDictionary(Dictionary **dict,char *key, char **val, int num_val)`
Η συνάρτηση αυτή χρησιμοποιείται για την ανανέωση ενός παλιού key στο dictionary. Αρχικά προσπελάυνεται κάθε κόμβος της λίστας ξεχωριστά, μέχρι να βρεθεί ο κόμβος που αντιστοιχεί στο κλειδί key, που έχει δεχθεί ως παράμετρο η συνάρτηση. Αν βρεθεί το ίδιο κλείδι σε κάποιον κόμβο τότε διαγράφονται από την μνήμη τα παλιά του στοιχεία, και αντικαθαστόνται από τις αντίστοιχες παραμέτρους της συνάρτησης, ενώ στο τέλος επιστρέφεται η τιμή 1, όπου σημαίνει ότι ενημερώθηκε το παλιό dictionary_node. Αν δεν βρεθεί τότε επιστρέφει την τιμή -1, δηλαδή απέτυχε η ενημέρωση του κλειδιού key στην μνήμη.
#### `void printDictionary(Dictionary *dic)`
Η συνάρτηση αυτή χρησιμοποιείται για την εκτύπωση των στοιχείων ενός Dictionary. Τυπώνει πρώτα το spec_id *aka dict_name* και προσπελαύνει κάθε κόμβο της λίστας ξεχωριστά και εκτυπώνει τα key - value του κάθε κόμβου.
#### `char **lookUpDictionary(Dictionary *dic, char *key, int *num_val)`
Η συνάρτηση αυτή χρησιμοποιείται για να δούμε αν υπάρχει το key στο Dictionary dic, και αν υπάρχει επιστρέφει τον πίνακα values του.
#### `void deleteDictionary(Dictionary **dic)`
Η συνάρτηση αυτή χρησιμοποιείται για την σωστή απελευθέρωση του dictionary από την μνήμη. Προσπελαύνει κάθε κόμβο της λίστας διαγράφοντας κατάλληλα τα στοιχεία του, και τέλος απελευθερώνει τα δικά του στοιχεία `(*lista, dict_name)` και απελευθερώνεται και το αντικείμενο Dictionary.
#### `int sizeDictionary(Dictionary *dict)`
Η συνάρτηση αυτή επιστρέφει το μέγεθος ενός Dictionary, όπου είναι ο αριθμός των κόμβων της λίστας.
#### `Dictionary *concatenateAllDictionary(Dictionary *dict)`
Αυτή η συνάρτηση προσπελαύνει όλες τις συμβολοσειρές του dictionary και τις αποθηκεύει σε έναν κόμβο. Έτσι ώστε να μπορεί να χρησιμοποιηθεί από το Bag Of Words.
## Bucket.h
### Bucket
Είναι μια δομή κόμβου λίστας όπου χρησιμοποιείται για την υλοποίηση της δομής της κλίκας. Όπου στον array `Dictionary **spec_ids` αποθηκεύουμε τα spec_ids τα οποία ταιριάζουν σύμφωνα με το csv και αν δεν χωρέσουν στον array δημιουργείτε καινούργιος κόμβος λίστας για να περάσουμε τα υπόλοιπα `Dictionary **spec_ids` και ούτω καθεξής. Χρησιμοποιείται ο ακέραιος `int cnt` για την καταμέτρηση των περασμένων spec_ids στον πίνακα του κόμβου, ο ακέραιος `int numofSpecs` όπου μας δείχνει το μέγιστο αριθμό των spec_ids που χωράει ο πίνακας στον κόμβο.
### Bucketlist 
Είναι μια δομή λίστας όπου αποθηκεύονται όλα τα spec_ids του αρχείου που ταιριάζουν μεταξύ τους. Στον ακέραιο `int num_entries` αποθηκεύουμαι τον αριθμό των spec_ids που έχουν αποθηκευτεί. Τον χαρακτήρα `char dirty_bit` όπου χρησιμοποιείται ως flag για να γνωρίζουμε αν πρέπει να τυπώσουμε τα ζευγάρια της κλίκας που βρίσκονται στη λίστα ή όχι. Ακόμη υπάρχουν δύο δείκτες σε κόμβους λίστας, όπου ο ένας δείχνει στην κεφαλή και ο άλλος στο τέλος της, `struct Bucket *head` και `struct Bucket *tail`. Προστέθηκε `secTable *negatives` που είναι μια δομή όπου αποθηκεύονται οι αρνητικές συσχετίσεις αυτής της κλίκας και εν τέλει δείχνουν στις `BucketList` κάποιας αρνητικής τους κλίκας. Έτσι όλες οι θετικά συσχετισμένες κλίκες, συσχετίζονται αρνητικά με όλες τις αρνητικές συσχετίσεις που μπορεί να έχουν. 
### Functions
#### `Bucket *Bucket_Create(Dictionary *spec_id, int BucketSize)`
Η συνάρτηση αυτή χρησιμοποιείται για τη δημιουργία ενός κόμβου λίστας, κάνοντας allocate τον κατάλληλο χώρο, και εισάγει το spec_id στον array των Dictionay για τα spec_ids. Το numofSpecs ορίζεται από το ΒucketSize.
#### `Bucket *Bucket_Insert(Bucket *buck, Dictionary *spec_id)`
Η συνάρτηση αυτή χρησιμοποιείται για να εισαχθεί ένα spec_id στον Dictionary array, αν όμως έχει γεμίσει ο array, τότε δημιουργείται καινούργιος κόμβος λίστας Bucket όπου εισάγεται στην αρχή της λίστας, ενώ ο δείκτης next του καινούργιου κόμβου δείχνει στην αρχική λίστα buck. Επιστρέφεται η ενημερωμένη λίστα.
#### `void Bucket_Delete(Bucket **DestroyIt,int mode)`
Η συνάρτηση αυτή χρησιμοποιείται για να διαγράψει το κόμβο της λίστας Bucket, η παράμετρος mode χρησιμοποιείται για να μην διαγραφούν από την μνήμη τα Dictionaries αν τα χρειάζομαστε, ενώ όταν φτάνουμε στο τέλος του προγράμματος διαγράφονται τα dictionaries από την μνήμη, μαζί με τον array τους και απελευθερώνεται ο χώρος που είχε δεσμευτεί από τον Bucket. 
#### `void Bucket_Print(Bucket *buck)`
Η συνάρτηση αυτή χρησιμοποιείται για να εκτυπώσει τα spec_ids που είναι μέσα στο Bucket.
#### `BucketList *BucketList_Create(Dictionary *spec_id, int BucketSize)`
Η συνάρτηση αυτή χρησιμοποιείται για να δημιουργήσει μία δομή BucketList. Δεσμεύει τον κατάλληλο χώρο για το BucketList, δημιουργεί τον πρώτο κόμβο της λίστας Bucket, αναθέτει κατάλληλα τους δείκτες της κεφαλής και της ουράς της λίστας, θέτει το dirty_bit σε 0 (μην το τυπώσεις) αφού περιέχει μία κλίκα και το num_entries σε 1.
#### `BucketList *BucketList_Insert(BucketList *buck, Dictionary *spec_id)`
Η συνάρτηση αυτή χρησιμοποιείται για να εισάγει ένα spec_id στη λίστα buck. Στην κεφαλή της λίστας χρησιμοποίειται η συνάρτηση Bucket_Insert και αυξάνεται ο αριθμός των num_entries.
#### `int BucketList_Bucket_Full(BucketList *bl)`
Η συνάρτηση αυτή επιστρέφει BUCKET_FIRST_FULL αν είναι γεμάτο ή BUCKET_FIRST_AVAILABLE αν υπάρχει χώρος.
#### `int BucketList_Empty(BucketList *b)`
Η συνάρτηση αυτή επιστρέφει LIST_EMPTY αν ο δείκτης κεφάλης της λίστας δείχνει σε ΝULL, αλλιώς επιστρέφει LIST_NOT_EMPTY.
#### `Dictionary *Bucket_Get_FirstEntry(BucketList *b)`
Η συνάρτηση αυτή επιστρέφει το πρώτο dictionary που υπάρχει στον πρώτο κόμβο της λίστας BucketList.
#### `void BucketList_Delete(BucketList **b, int mode)`
Η συνάρτηση αυτή χρησιμοποιείται για την διαγραφή της δομής BucketList και την κατάλληλη απελευθέρωση της μνήμης που έχει δεσμεύσει η δομή αυτή. Αρχικά ελέγχει αν η λίστα είναι NULL. Αυτό το ενδεχόμενο συμβαίνει διότι διαγράφονται όλοι οι κόμβοι του HashTable που περιέχουν τους δείκτες στη λίστα, και με την υλοποίηση μας μπορεί μία λίστα να έχει ήδη διαγραφεί. Επομένως αν υπάρχει τότε προσπελαύνεται κάθε κόμβος της λίστας ξεχωρίστα διαγράφεται ο κόμβος αυτός, ανάλογα και με το mode, και τέλος απελευθερώνεται και η μνήμη που έχει ανατεθεί στη λίστα BucketList.
#### `void BucketList_Print(BucketList *b)`
Η συνάρτηση αυτή χρησιμοποιείται για την εκτύπωση των spec_ids των κόμβων της λίστας.
#### `BucketList *BucketList_Delete_First(BucketList **b,int mode)`
H συνάρτηση αυτή χρησιμοποιείται για διαγράψει τον πρώτο κόμβο της λίστας b. Αν υπάρχει ένας κόμβος τότε διαγράφεται κανονικά και επιστρέφεται ΝULL. Αλλιώς διαγράφεται ο πρώτος κόμβος κανονικά, και αναθέτουμε τον δείκτη του κόμβου κεφαλής στον επόμενο κόμβο από αυτόν που διαγράψαμε. Τέλος επιστρέφεται η λίστα b χωρίς τον αρχικό της κόμβο.
#### `void bucketListWriteCliques(BucketList *lista, FILE *fp)`
Η συνάρτηση αυτή χρησιμοποιείται για να προσπελαύσει κάθε στοιχείο της λίστας lista και να τα τυπώσει στο αρχείο fp. Ξεκινώντας από το πρώτο αντικείμενο Dictionary της λίστας το θεωρούμε ως left_spec_id, εν συνέχεια προπελάυνουμε τα υπόλοιπα στοιχεία της λίστας, ένα-ένα τα θεώρουμε right_spec, τα τυπώνουμε στο αρχείο, συνεχίζουμε στο βρόγχο εώς ότου left_spec_id γίνουν όλα τα αντικείμενα της λίστας πέρα από το τελευταίο. Έτσι ώστε κάθε ζευγάρι κλίκας να τυπωθεί ακριβώς μία φόρα.
#### `BucketList *updateNegativeRelations(BucketList *to_upd, BucketList *to_replace)`
Όταν γίνεται merge 2 κλίκων τότε θα υπερισχήσει ένα BucketList, όποτε όλες οι αρνητικές συσχετίσεις του `to_upd`, πρέπει να δείχνουν στο `to_replace`. Οπότε η συνάρτηση προσπελάβνει όλες τις αρνητικές συσχετίσεις του `to_upd` και στις `negatives` των αρνητικών διαγράφει το δείκτη προς το `to_upd` και τον αντικαθιστά με αυτόν του `to_replace`, αν αυτός ήδη υπάρχει, δεν τον περνάει 2η φορά.
#### `BucketList *bucketListWriteNegativeCliques(BucketList *lista,FILE *fp)`
Αυτή η συνάρτηση προσπελάυνει όλες τις θετικές συσχετίσεις και όλες τις αρνητικές συσχετίσεις που υπάρχουν στη λίστα, και δημιουργεί όλα τα πιθανά αρνητικά ζευγάρια, και τα καταγράφει στο αρχείο `fp`. Επιστρέφει `BucketList *` γιατί κάνει update το `dirty_bit` που ελέγχει αν κάποια κλίκα έχει ήδη γραφτεί στο αρχείο.
#### `BucketList* mergeNegativeRelations(BucketList *to_merge, BucketList **to_del)`
Δύο θετικές κλίκες που είναι να γίνουν merge, πρέπει να γίνουν merge και οι αρνητικές τους συσχετίσεις. Οπότε προσπελάυνονται μία - μία οι αρνητικές συσχετίσεις τoυ `to_del` αφαιρώντας τες και τις κάνουμε `insert_SecTable()` στις αρνητικές συσχετίσεις του `to_merge`.
#### `BucketList *updateSec(BucketList *re, BucketList *to_delete, BucketList *to_replace)`
Αυτή η συνάρτηση χρησιμοποιείται για την ενημέρωση της δομής των αρνητικών συσχετίσεων τις `re`. Αυτή έδειχνε στο `to_delete`, αλλά επειδή τώρα θα διαγραφεί και όλες οι κλίκες του θα πάνε στο `to_replace`, εμείς πρέπει είτε να το διαγράψουμε αν υπάρχει στις αρνητικές συσχετίσεις του `re` το `to_replace`, ή να το αντικαταστήσουμε.
#### `void rightSpecNegativeCliques(BucketList *set, char *left_sp, FILE *fp)`
Aυτή η συνάρτηση χρησιμοποιείται για να φτιάξει σωστά όλα τα πιθανά αρνητικά ζευγάρια και να τα τυπώσει στο αρχείο.
### HashTable.h
#### keyBucketEntry
Η δομή αυτή περιέχει ένα ξεχωριστό spec_id ως `char *key` και μία λίστα `BucketList *set` όπου περιέχει όλες τα υπόλοιπα spec_ids που ταιριάζει το key.
#### keyBucket
Η δομή αυτή περιέχει ένα `int bucket_size` που καταχωρύται το μέγεθος του Bucket σε bytes, τα `int num_entries` όπου είναι ο αριθμός των καταχωρυμένων keyBucketEntry στον array `keyBucketEntry **array` και `int max_entries` όπου είναι ο αριθμός των keyBucketEntry που μπορούν να καταχωρυθούν στον array.
#### HashTable
Η δομή αυτή είναι ένα HashTable το οποίο αναδιαμορφώνει το μέγεθος του και το διπλασιάζει όταν δεν υπάρχει χώρος σε ένα bucket. Ως `int bucket_num` καταχωρούνται ο μέγιστος αριθμός buckets που μπορεί να έχει το Table. Και ως `keyBucket **table` είναι ο array που περιέχει τα στοιχεία του HashTable, στη θέση μνήμης που έχει υποδείξει η HashFuction.
#### Functions
#### `unsigned long hashCode(char *,int)`
Αυτή η συνάρτηση χρησιμοποιείται για να hash-άρει τα spec_ids και επιστρέφει τη θέση τους στον πίνακα.
#### `keyBucket *initKeyBucket(Dictionary *spec_id)`
Η συνάρτηση χρησιμοποιείται για να δημιουργήσουμε μια δομή keyBucket. Δεσμεύεται η κατάλληλη μνήμη, αρχικοποιούνται κατάλληλα τα στοιχεία της, και δημιουργείται το πρώτο keyBucketEntry στη θέση 0 του array, αφού έχει δεσμευτεί η μνήμη και έχει δημιουργηθεί ο πίνακας με αριθμό στοιχείων max_entries. Όλα τα στοιχεία του array δείχνουν σε NULL εκτός από το πρώτο, που δείχνει στο νέο μας KeyBucketEntry.
#### `int keyBucketAvailable(keyBucket *kb)`
Η συνάρτηση ελέγχει αν στο keyBucket kb υπάρχει χώρος. Αν δηλαδή τα max_entries είναι ίσα με τα num_entries. Επιστρέφει **KEY_BUCKET_FULL** αν ισχύει, αλλιώς **KEY_BUCKET_AVAILABLE**.
#### `keyBucketEntry *createEntry(Dictionary *dict)`
H συνάρτηση αυτή δεσμεύει τον χώρο κατάλληλα για να δημιουργηθεί ενα αντικείμενο keyBucketEntry. Αρχικοποιεί κατάλληλα κάθε μεταβλητή του keyBucketEntry, δημιουργώντας και τη λίστα BucketList όπου αποθηκεύεται το Dictionary dict.
#### `keyBucket *insertBucketEntry(keyBucket *kb,Dictionary *spec_id)`
Η συνάρτηση αυτή εισάγει ένα καινούργιο spec_id στη δομή, δημιουργώντας ένα ανιτκείμενο στο keyBucketEntry στο πρώτο άδειο κελί του πίνακα και αυξάνει τον αριθμό των num_entries της δομής.
#### `Dictionary *getTopKeyBucketEntry(keyBucket *kb, int pos)`
Επιστρέφει το πρώτο αντικείμενο της λίστας BucketList, από το keyBucketEntry που δείχνει ο πίνακας του keyBucket kb στη θέση pos.
#### `HashTable *reshapeHashTable(HashTable **ht,Dictionary *spec_id)`
Η συνάρτηση αυτή αναδιαμορφώνει το μέγεθος του HashTable στο διπλάσιο του. Αρχικά δημιούργειται ένα νέο HashTable, με το διπλάσιο μέγεθος από το παλιό. Έπειτα προσπελαύνεται κάθε κελί του παλιού HashTable και λαμβάνει όλα τα ήδη αποθηκευμένα Dictioanry και τα ξαναπερνάει στο καινούργιο HashTable. Έπειτα περνάει το spec_id που έκανε το collision και διαγράφει το παλιό hashTable, χωρίς να διαγράψει τα dictionary. Επιστρέφεται το καινούργιο HashTable με το διπλάσιο μέγεθος.
#### `HashTable *initHashTable(int buckets_num)`
Η συνάρτηση αυτή δεσμεύει χώρο στη μνήμη για να δημιουργήσει μια δομή HashTable, αρχικοποιεί κατάλληλα τα στοιχεία της και επιστρέφει το αντικείμενο HashTable, buckets_num είναι ο αριθμός των κελιών του, ενώ το bucketSize το έχουμε κάνει define.
#### `HashTable *insertHashTable(HashTable **ht,Dictionary *spec_id)`
Η συνάρτηση αυτή hasharei το spec_id, και το αποθηκεύει στο HashTable. Αν το bucket που επιστρέφεται από την Hash Function είναι γεμάτο τότε καλείται η reshapeHashTable() για να γίνει αναδιαμόρφωση του HashTable, έτσι ώστε να υπάρχει χώρος. Αλλιώς 
αν το Bucket υπάρχει τότε το αποθηκεύουμαι στο Bucket, αν δεν υπάρχει τότε δημιουργούμε το Bucket.
#### `int sizeHashTable(HashTable *ht)`
Η συνάρτηση αυτή αθροίζει διαδοχικά τα Num_entries των buckets που υπάρχουν στο HashTable και επιτρέφει τον ακέραιο που υπολόγισε.
#### `int findKeyBucketEntry(HashTable *ht,char * spec_id)`
Η συνάρτηση αυτή hasharei την τιμή του spec_id βρίσκει τον αριθμό σε ποιο κελί του HashTable βρίσκεται και επιστρέφει σε ποιο σημείο του κελιού βρίσκεται η τιμή spec_id. Αν το κελί δεν υπάρχει ή το spec_id τότε επιστρέφεται η τιμή -1.
#### `void deleteHashTable(HashTable **destroyed,int mode)`
H συνάρτηση αυτή απελευθερώνει τη μνήμη, που έχει δεσμευτεί για το HashTable. Προσπελαύνει κάθε κελί του και αν υπάρχει διαγράφεται από τη μνήμη, έπειτα απελευθερώνεται ο πίνακας και τέλος το HashTable.
#### `void cliqueDeleteHashTable(HashTable **ht,int mode)`
Η συνάρτηση αυτή χρησιμοποιείται για την διαγραφή του HashTable, αφ'ότου οι κλίκες έχουν δημιουργηθεί. Ουσιαστικά αυτή η συνάρτηση προσπελαύνει κάθε κέλι του Bucket, και από εκεί προσπελαύνεται κάθε στοιχείο του array του, και ελέγχεται αν η λίστα που δείχνει keyBucketEntry έχει αποθηκευμένες περισσότερες από μία κλίκες, αν όχι τότε διαγράφεται από την μνήμη. Αν ναι τότε βρίσκονται όλα τα keyBucketEntry -> set που δείχνουν σε αυτή τη κλίκα και το set τους γίνεται ΝULL, έπειτα διαγράφεται η λίστα. Διαγράφονται κανονικά τα υπόλοιπα στοιχεία του HashTable, και τέλος το HashTable. 
#### `HashTable *erasePointerHashTable(HashTable **ht,keyBucketEntry *clique_bucket)`
Η συνάρτηση αυτή χρησιμοποιείται για να αναθέσει όλου τους δείκτες σε μία κλίκα την τιμή NULL, έτσι ώστε η κλίκα να διαγραφεί επιτυχώς. Προσπελαύνει ξεχωριστά τους κόμβους της λίστας set, γίνεται χρήση των συναρτήσεων hashCode() και findKeyBucketEntry() για να βρεθεί η θέση του pointer set του κάθε κόμβου στο HashTable και να τον θέσουμε σε ΝULL.
#### `void deleteKeyBucket(keyBucket **destroyed,int mode)`
Η συνάρτηση αυτή απελευθερώνει κατάλληλα τον χώρο που έχει δεσμευτεί από την δημιουργία της δομής keyBucket. Προσπελαύνει τα στοιχεία του πίνακα, και αν υπάρχουν τα διαγράφει, έπειτα διαγράφει τον πίνακα και τέλος το keyBucket.
#### `void printHashTable(HashTable *ht)`
Η συνάρτηση αυτή χρησιμοποιείται για να τυπώσει τα στοιχεία ενός HashTable.
#### `HashTable *createCliqueHashTable(HashTable **ht, char *left_sp,char *right_sp)`
Αυτή η συνάρτηση χρησιμοποιείται για να δημιουργήσει τις κλίκες στο HashTable. Αρχικά βρίσκονται οι θέσεις των left_spec και right_spec στο HashTable, ελέγχεται αν δεν έχουν ήδη καταχωρυθεί επαγωγικά στην ίδια λίστα, κοιτάμε πιο έχει τα περισσότερα num_entries. Επιλέγουμε αυτό που έχει τα λίγοτερα και αλλάζουμε τους δείκτες των στοιχείων που είναι κλίκες με αυτό να δείχνουν στην καινούργια λίστα changePointerHashTable() που θα γίνει με το BukcetListMerge(). Έπειτα γίνεται το ΒucketListMerge() των δύο λιστών και επιστρέφεται ανανεωμένο το HashTable.
#### `HashTable *changePointerHashTable(HashTable **ht,keyBucketEntry *old_bucket,keyBucketEntry *new_bucket)`
Η συνάρτηση αυτή χρησιμοποιείται για να αλλάξουμε σωστά τους δείκτες στην νέα λίστα που θα διαμορφωθεί από το merge. Προσπελαύνουμε όλα τα στοιχεία που βρίσκονται στην clique_bucket βρίσκουμε τις θέσεις τους στο HashTable και ο δείκτης της λίστας τους set αλλάζει και ορίζεται να δείχνει εκεί που θα βρίσκεται η merged List. Κατα τη διάρκεια του βρόγχου παρακάμπτεται το old_bucket και δεν αλλάζουμε τον δείκτη του.
#### `BucketList *BucketList_Merge(BucketList **Max_List, BucketList **min_List,HashTable **ht,int h,int index)`
Η συνάρτηση αυτή ενώνει δύο διαφορετικές λίστες μεταξύ τους. Στην μία περίπτωση, όπου η λίστα που min_List (η λίστα με τα λιγότερα num_entries) είναι γεμάτος ο κόμβος της κεφαλής της, προστίθεται στο τέλος της Max_list απελευθερώνεται ο χώρος που είχε δεσμευτεί από το δείκτη της min_list, το hashTable δείχνει σωστά στη νέα λίστα και επιστρέφεται η λίστα. Αλλιώς προσπελαύνεται κάθε στοιχείο της κεφαλής min_list και προστίθεται στη Max_List. Ο πρώτος κόμβος της min_list διαγράφεται. Αν η min_list άδειασε διαγράφουμε την υπόλοιπη λίστα θέτουμε τους δείκτες του HashTable να δείχνουν σωστά και επιστρέφουμε τη λίστα. Αλλιώς κάνουμε το ίδιο που κάναμε στην αρχική μας περίπτωση αφού τώρα θα έχουμε μία γεμάτη min_list.
#### `HashTable *negativeRelationHashTable(HashTable *ht, char *left_sp,char *right_sp)`
Η συνάρτηση αυτή χρησιμοποίειται για να εισάγει μία αρνητική συσχέτιση μεταξύ του `left_sp` και του `right_sp`.
## LogisticRegression.h
#### logisticreg
Η δομή είναι μια απλή δομή μοντέλου logistic Regression. `int numOfN` είναι ο αριθμός των weights. `double *vector_weights` είναι ο πίνακας με τις τιμές των weights. `double learning_rate` είναι το "η", learning rate. `int steps` είναι ο αριθμός επαναλήψεων για ένα train session. `int batches` είναι ο αριθμός των διανυσμάτων που θα χρησιμοποιηθούν για κάποιο train. `int ratio` είναι ο αριθμός των επαναλήψεων που θα γίνουν όταν το αποτέλεσμα ενός διανύσματος είναι το 1.
#### Functions
#### `logisticreg *fit_logisticRegression(logisticreg *model,double **X,int *y,int low,int high)`
Η συνάρτηση εφαρμόζει τον μαθηματικό τύπο της logisticRegression για έκεινη τη στιγμή. To low και το high καθορίζονται από το batch, που είναι ουσιαστικά, το με πόσα διανύσματα θα γίνει η εφαρμογή του τύπου.
#### `logisticreg *create_logisticReg(int numofN,int mode,int steps,int batches,double learning_rate,int ratio)`
Δημιουργεί το μοντέλο στη μνήμη.
#### `logisticreg *create_logisticReg_fromFile(char *filename,char **sigmod_filename, char **bow_type, int *vector_type)`
Διαβάζει ένα αρχείο, στην περιπτώση μας το *stats.txt*, και δημιουργεί το μοντέλο από εκεί. 
#### `double *vectorize(double *x, double *y, int numofN,int type)`
Δημιουργεί ένα διάνυσμα μεταξύ των left_spec_bagOfWords και right_spec_bagOfWords, ανάλογα με τον τύπο καλείται η το `absolute_distance` ή `concatenate_vectors`. 
Αυτό το διάνυσμα αποθηκεύεται σε έναν πίνακα και χρησιμοποιείται και για το training και για το inference.
#### `double *absolute_distance(double *x, double *y, int numofN)`
Επιστρέφεται η απόλυτη απόσταση 2 διανυσμάτων.
#### `double *concatenate_vectors(double *x,double *y, int numofN)`
Επιστρέφεται η ένωση 2 διανυσμάτων.
#### `double *predict_logisticRegression(logisticreg *model,double **X,int train,int n)`
Yπολογίζεται ξεχωριστά το άθροισμα για κάθε ξεχωριστό διάνυσμα του X, το απότελεσμα του κάθε αθροίσματος αν δοθεί ως όρισμα στη `sigmoid` είναι η προβλεπόμενη τιμή. Επιστρέφονται όλες οι προβλεπόμενες τιμές.
#### `double loss_LogisticRegression(logisticreg *model, double *xi,int yi)`
Επιστρέφεται το error της τιμής που προβλέψαμε.
#### `double sigmoid(double t)`
Είναι η εφαρμογή του μαθηματικού τύπου της σιγμοιδής.
#### `logisticreg *train_logisticRegression(logisticreg *model,double **X,int *y,int size)`
Η συνάρτηση χωρίζει σε μικρότερους πίνακες τα διανύσματα που περιέχει ο πίνακας X (batch) και καλεί την `fit_logisticRegression` για να κάνει train το μοντέλο. 
#### `void printStatistics(logisticreg *model,char *filename,char *bow_type, int vector_type)`
H συνάρτηση αυτή τυπώνει σε ένα αρχείο ό,τι χρησιμοποιήθηκε κατα τη διάρκεια της διαδικασίας του training. Έτσι ώστε να μπορεί διαφορετική main να το διαβάσει και να δημιουργήσει το μοντέλο εκπαιδευμένο.
#### `void delete_logisticReg(logisticreg **del)`
Διαγράφει το μοντέλο και απελευθερώνει κατάλληλα τη μνήμη.
## Metrics.h
#### metrics
Ουσιαστικά η δομή αυτή αποθηκεύει και υπολογίζει το recall, το precision και το f1_score του μοντέλου μας.
## SecTable.h
#### secondaryNode
Είναι μια απλή δομή κόμβου λίστας στην οποία αποθηκεύονται σε πίνακα `int num_elements` στοιχεία `void **values`, δηλαδή ή θα είναι Pointer, αλλιώς διεύθυνση μνήμης για τις αρνητικές συσχετίσεις, ή θα είναι String ή θα είναι indexedWord. Υπάρχει ένας δείκτης `secondaryNode *next` για τον επόμενο κόμβο.
#### secondaryTable
Είναι ένα Hash Table, στο οποίο το υλοποιήσαμε με τέτοιο τρόπο έτσι ώστε να μπορεί να χρησιμοποιηθεί για να αποθηκεύουμε διαφορετικά δεδομένα στον κώδικα μας. `Data type` είναι ο τύπος του στοιχείου που έχει αποθηκευτεί στη δομή. `float loadFactor` είναι η μεταβλητή για το πότε θα κάνει reshape το Hash Table. `int numOfBuckets` είναι ο αριθμός των κουβάδων του Hash Table. `int num_elements` είναι ο τρέχων αριθμός στοιχείων που έχει αποθηκευτεί στο Hash Table. `int bucketSize` είναι το μέγεθος του κάθε κουβά. `Hash hashFunction` είναι δείκτης σε συνάρτηση κατακερματισμού, που βοηθάει, όταν έχουμε διαφορετικούς τύπους στοιχείων. Ομοίως έχουμε `Compare cmpFunction`, `Delete deleteFunction` οι οποίες αντίστοιχα συγκρίνουν και διαγράφουν στοιχεία του HashTable. `secondaryNode **table` είναι ο πίνακας στον οποίο αποθηκεύονται τα στοιχεία μας.
#### Functions
#### `secTable *create_secTable(int size, int bucketSize, Hash hashFunction, Compare cmpFunction,Delete deleteFunction,Data type)`
H συνάρτηση αυτή δημιουργεί και αρχικοποιεί έναν πίνακα κατακερματισμού secTable.
#### `secTable *insert_secTable(secTable *st, void *value)`
Η συνάρτηση αυτή αποθηκεύει το αντικείμενο value στο secTable.
#### `secTable *replace_secTable(secTable *st, void *old_value, void *new_value)`
Η συνάρτηση αυτή βρίσκει την παλιά εγγραφή στο secTable και την αντικαθιστά με την καινούργια. Αν υπάρχει ήδη απλά διαγράφεται η παλιά εγγραφή. Αλλιώς διαγράφεται η παλιά και εισάγεται η καινούργια.
#### `secTable *reshape_secTable(secTable **st)`
H συνάρτηση αυτή χρησιμοποιείται για την αναδημιουργία του Hash Table. Όταν υπάρξει overflow στο HashTable, πάμε και το δημιουργούμε από την αρχή και ξανα εισάγουμε τις εγγραφές μας καλύτερα στα Bucket του.
#### `int findNextPrime(int N)`
Η συνάρτηση αυτή βρίσκει τον επόμενο πρώτο αριθμό μετά το Ν. Αυτή η συνάρτηση χρησιμεύει στην αναζήτηση του επόμενου βέλτιστου μεγέθους για το Hash Table.
#### `int getNumElements_secondaryNode(secondaryNode *node)`
Αυτή η συνάρτηση επιστρέφει τον ακέραιο αριθμό των στοιχείων που περιέχονται στο πρώτο κόμβο της αλυσίδας node.
#### `secondaryNode *create_secondaryNode(void *value,int size)`
Αυτή η συνάρτηση δημιουργεί και αρχικοποιεί ένα αντικείμενο κόμβου secondaryNode, εισάγοντας και το πρώτο στοιχείο value.
#### `secondaryNode *getFirstVal(secondaryNode *node, void **value)`
Επιστρέφει το πρώτο στοιχείο του κόμβου, node.
#### `void destroy_secondaryNode(secondaryNode **node,Delete deleteFunction,int mode)`
Καταστρέφει και απελευθερώνει τη μνήμη που είχε δεσμεύσει ο κόμβος και τα στοιχεία που αποθήκευε. Τo mode παίρνει τιμές `ST_SOFT_DELETE_MODE` ή `ST_HARD_DELETE_MODE`, αντίστοιχα δεν διαγράφονται ολικώς τα δεδομένα, αλλά καταστρέφεται η δομή, ή διαγράφεται η δομή με τα δεδομένα της.
#### `secTable *deletevalue_secTable(secTable *st, void *value,int mode)`
Διαγράφεται από το secTable το στοιχείο αυτό. Ανάλογα και με το mode, `ST_SOFT_DELETE_MODE` : διαγράφεται μόνο από τη δομή ή `ST_HARD_DELETE_MODE` : διαγράφεται και από τη μνήμη.
#### `secondaryNode *deletevalue(secondaryNode *node, void *value, Compare compareFunction,Delete deleteFunction,int mode)`
Η συνάρτηση αυτή διαγράφει το στοιχείο value από τον κόμβο, ισχύει ότι ισχύει παραπάνω.
#### `void destroy_secTable(secTable **st,int mode)`
Απελευθερώνει και διαγράφει όλη τη δομή secTable από τη μνήμη, το mode είναι ίδιο, οπώς και στις υπόλοιπες delete και destroy συναρτήσεις.
#### `int find_secTable(secTable *st,void *value)`
Βρίσκει αν κάποιο στοιχείο υπάρχει στο secTable, επιστρέφει 1 αν υπάρχει, αλλιώς 0.
#### `int find_secondaryNode(secondaryNode *node,void *value,Compare compare_func)`
Βρίσκει αν κάποιο στοιχείο υπάρχει στο secondaryNode, επιστρέφει 1 αν υπάρχει, αλλιώς 0.
#### `void *getIndexedWord_secTable(secTable *st,char *value)`
Βρίσκει αν υπάρχει η λέξη value αποθηκευμένη ως indexedWord στη δομή. Αν ναι επιστρέφει την indexedWord, αλλιώς επιστρέφει NULL.
#### `void *getIndexedWord_secondaryNode(secondaryNode *node,char *value,Compare compare_func)`
Βρίσκει αν υπάρχει η λέξη value αποθηκευμένη ως indexedWord στη δομή. Αν ναι επιστρέφει την indexedWord, αλλιώς επιστρέφει NULL.
#### `void *getIndexedWord_secTable(secTable *st,char *value)`
H συνάρτηση αυτή επιτρέφει μια δομή indexedWord για το δοσμένο value χρησιμοποιώντας τη συνάρτηση getIndexedWord_secondaryNode() η οποία αναζητά σε έναν bucket του secTable την το indexedWord το οποίο έχει σαν πεδίο word το string value.
#### `secTable *updateTF_secTable(secTable *st,void *value)`
Η συνάρτηση αυτή αναζητά ένα indexedWord σαρώνοντας το hash table και αν το βρει τότε αυξάνει το πεδίο tf αυτού του indexed word κατα 1.
#### `secTable *update_tf_idf_values(secTable *st,secTable *unique_words,int text_len)`
Η συνάρτηση αυτή σαρώνει το hash table και χρησιμοποιώντας τη συνάρτηση update_tf_idf_word() αναβαθμίζει τις τιμές στα πεδία idf και tf_idf_mean κάθε λέξης που βρίσκεται μέσα στο table unique_words το οποίο περιέχει όλες τις μοναδικές λέξεις οι οποίες βρέθηκαν στο json αρχείο του οποίου η επεξεργασία τελείωσε.
#### `secTable *update_tf_idf_word(secTable *st,char *value,int text_len)`
Η συνάρτηση αυτή ψάχνει τη λέξη value μέσα σε ένα bucket του secTable και όταν τη βρει αυξάνει το πεδίο idf κατα 1, και προσθέτει στο πεδίο tf_idf_mean το tf ( text frequency ) τις λέξεις για το json για το οποίο κλήθηκε η update_tf_idf_values, ώστε να υπολογιστούν όλες οι τιμές tf της κάθε λέξης για όλα τα αρχεία. Η τιμή αυτή θα χρησιμοποιηθεί αφού τελειώσει η επεξεργασία όλων των αρχείων json για να υπολογιστεί το μέσο tf_idf κάθε λέξης. Στη συνέχεια το πεδίο tf τις λέξεις μηδενίζεται ώστε να γίνει ο υπολογισμός του για το επόμενο json.
#### `secTable *evaluate_tfidf_secTable(secTable *vocabulary,int num_texts)`
Η συνάρτηση αυτή καλείται αφού ολοκληρωθεί το parsing σε όλα τα json αρχέια. Στο όρισμα num_texts βρίσκεται ο αριθμός των json αρχείων. Η συνάρτηση αυτή λοιπόν σαρώνει το secTable και για κάθε εγγραφή του ( μόνο αν οι εγγραφές είναι indexedWords ) υπολογίζει την τελική τιμή idf παίρνοντας το λογαριθμό  με βάση δέκα τις διαίρεσεις (num_texts/τα κείμενα στα οποία έχει εφμανιστεί η λέξη) , ο οποίος αριθμός μέχρι τώρα ήταν αποθηκευμένος στο idf πεδίο κάθε λέξεις. Ο λογάριθμος αυτός αποθηκεύται τελικά στο πεδίο idf κάθε λέξεις. Στη συνέχεια υπολογίζεται το μέσο tf_idf αφού στο πεδίο tf_idf_mean κάθε λέξης βρίσκεται το άθροισμα όλων των tf τιμών των λέξεων στα json, άρα στο πεδίο tf_idf_mean αποθηκεύεται η πράξη `(idf*tf_idf_mean/num_texts)` που είναι το μέσο tf_idf score τις λέξεις. Στη συνέχεια ελέγχεται αν αυτό το score είναι μεγαλύτερο απο μια σταθερά την MIN_TF_IDF και αν είναι η λέξη αποθηκεύται σε ένα καινούριο hash table. Αυτή η διαδικασία επαναλαμβάνεται για κάθε λέξη και στο τέλος επιστρέφεται το καινούριο hash table το οποίο περιέχει τις λέξεις με τα καλύτερα μέσα tf_idf.
#### `void writeVocab_secTable(secTable *st)`
Η συνάρτηση αυτή γράφει στο αρχείο `vocabulary.txt` το vocabulary που χρησιμοποιήθηκε για το training. Έτσι ώστε στο στάδιο του inference, να μην γίνει inference με διαφορετικό vocabulary.
#### `secTable *initVocab_secTable(char *filename)`
Η συνάρτηση αυτή διαβάζει ένα αρχείο με όνομα `filename` και δημιουργεί το αντίστοιχο vocabulary, που είχε αποθηκευτεί στο αρχείο.
# Υπόλοιπες συναρτήσεις
## CsvReader.h
#### `HashTable *csvParser(char *filename, HashTable **ht)`
Αυτή η συνάρτηση parsa - ρει τις κλίκες από το filename. Διαβάζοντας κάθε γραμμή, τη σπάει σε 3 στοιχεία από το ',' και αν το 3ο είναι 1 τότε περνάει την κλίκα στο HashTable με την συνάρτηση createCliqueHashTable(), αλλιώς χρησιμοποιεί τη `negativeRelationHashTable()` και δημιουργεί την αρνητική συσχέτιση, τέλος κλείνει το αρχείο που άνοιξε και αποδεσμεύει την μνήμη που δέσμευσε. Επιστρέφει το HashTable που δημιούργησε.
#### `void csvWriteCliques(HashTable **ht)`
Αυτή η συνάρτηση χρεισημοποείται για να δημιουργήσει ένα αρχείο *cliques.csv* και να εκτυπώσει όλες τις κλίκες που υπάρχουν στο HashTable προσπελαύνοντας το και εκτυπώνοντας μία φορά τα ζεύγη στη μορφή *left_spec_id,right_spec_id*
#### `void csvWriteNegativeCliques(HashTable **ht)`
Είναι παρόμοια με την `csvWriteCliques` απλά αυτή τη φορά δημιουργεί το αρχείο *neg_cliques.csv* και με τη βοήθεια της `bukcetListWriteNegativeCliques()` γράφονται όλα τα πιθανά ζευγάρια στο αρχείο που δημιουργήσαμε.
#### `void csvLearning(char *filename, HashTable *ht, secTable *vocabulary, int linesRead,char *bow_type,int vector_type,int ratio)`
Ανοίγει το αρχείο filename , και δημιουργώντας ένα αντικείμενο `logisticreg *regressor`, αρχίζει να γίνεται το train. Οι μεταβλητές `secTable *vocabulary` είναι το λεξιλόγιο που έχει δημιουργηθεί για το training, `int linesRead` οι γραμμές του αρχείου που θα γίνει το training, χωρίς να μετράμε την πρώτη, `char *bow_type` είναι ο τύπος του BagOfWord ή tf-idf, `int vector_type` αν θα χρησιμοποίησουμε την απόλυτη τιμή της απόστασης των διανυσμάτων `abs` ή την ένωση των διανυσμάτων `concat` και `int ratio` είναι ο αριθμός των επαναλήψεων που θα γίνει στο 1. Λαμβάνουμε το 80% του αρχείου για να κάνουμε το training. Έπειτα προσπελαύνουμε όλο το αρχείο και δημιουργούμε ένα τελικό διάνυσμα για κάθε γραμμή του αρχείου, αποθηκεύοντας τη σχέση σε έναν άλλο πίνακα. Ανακατεύονται τα δεδομένα. Μετά σπάμε τον πίνακα των διανυσμάτων σε κομμάτια, batches, και εφαρμόζουμε το `train_logisticRegression()`. Τέλος για το υπόλοιπο 20% του πίνακα βλέπουμε τα predictions που έκανε το μοντέλο μας, και υπολογίζουμε το f1_score του. Αποθηκεύεται σε ένα αρχείο *stats.txt* το εκπαιδευμένο μοντέλο και η συνάρτηση απελευθερώνει τη μνήμη που δημιούργησε και τερματίζει.
#### `void csvInference(char *filename, HashTable *ht, secTable *vocabulary, logisticreg *model, char *bow_type, int vector_type)`
H συνάρτηση αυτή ανοίγει το αρχείο `filename` και δημιοργεί έναν πίνακα διανυσμάτων όσο διαβάζει μία - μία τις γραμμές από το bagofWords τους. Έπειτα το μοντέλο μαντεύει κάθε τιμή y του left_spec,right_spec,y. Και τέλος τα τυπώνει σε ένα αρχείο *predictions.csv* και υπολογίζει το f1_score και το τυπώνει στην οθόνη.
## DataPreprocess.h
#### `secTable *init_stopwords()`
Δημιουργεί ένα secTable το οποίο περιέχει κάθε πιθανό stopWord το οποίο δεν θα πρέπει να το λάβουμε υπ'οψιν όταν δημιουργούμε το BOW.
#### `char *text_cleaning(char *text)`
Η συνάρτηση αυτή αφαιρεί από τη συμβολοσειρά τα "\n", τα σημεία στίξης και μετατρέπει τα κεφαλαία γράμματα σε μικρά.
#### `int check_utf16(char *word)`
Ελέγχει αν η λέξη είναι utf16.
#### `int single_character(char *word)`
Ελέγχει αν είναι ένας μόνο χαρακτηρακτήρας και όχι αριθμός.
#### `char *remove_stopwords(char *text,secTable *stopwords,secTable **vocabulary,secTable **unique_words,int *len)`
Η συνάρτηση αυτή αφαιρεί από τη συμβολοσειρά `text` όλα τα πιθανά stopwords, και αν δεν υπάρχει στο vocabulary τότε προστείθεται.
#### `char *preprocess(char *text,secTable *stopwords,secTable **vocabulary,secTable **unique_words,int *len)`
H συνάρτηση αυτή καλεί για τη συμβολοσειρά `text` τις παραπάνω συναρτήσεις, `text_cleaning` και `remove_stopwords`.
#### `void swap_int(int *a,int *b)`
Αλλάζει τις τιμές δύο μεταβλητών ακεραίων μεταξύ τους.
#### `void swap_vectors(double **a,double **b)`
Aλλάζει τις τιμές δύο μεταβλητών πινάκων μεταξύ τους.
#### `void swap_string(char **a,char **b)`
Αλλάζει τις τιμές 2 μεταβλητών συμβολοσειρών μεταξύ τους.
#### `int random_int(int n)`
Επιστρέφει έναν τυχαίο αριθμό μεταξύ του 0 - MAXLINES του αρχείου. 
#### `void shuffle_data(double **X,int *y,char **pairs,int n,int random_state)`
Μπερδεύει μεταξύ τους τους πίνακες που θα γίνει το training. Έτσι ώστε να έχουμε καλύτερα αποτελέσματα στο training.
## JsonParser.h
#### `char *get_datasetX_name()`
Η συνάρτηση αυτή προσπελαύνει όλα τα αρχεία στον τοπικό φάκελο και βρίσκει αυτό που περιέχει τα datasets. Αυτό γίνεται αφού ανοίγει τους υποφακέλους μέχρι να βρει το αρχείο που θα έχει format xxxx.json και επιστρέφει το path του αρχείου αυτού. 
#### `int Initialize_dataset_X(char *name,HashTable **ht)`
H συνάρτηση αυτή χρησιμοποιείται για να φτιάξει τις πρώτες κλίκες του HashTable. Αυτό γίνεται προσπελαύνοντας όλους του υποφακέλους του dataset_X, και επαναλαμβάνεται ένας βρόγχος ο οποίος προσπελαύνει τα αρχεία του υποφακέλου και τα parsa-ρει και τα αποθηκεύει στο HashTable ht με τη συνάρτηση parse_json_file(). Αν όλα πήγαν καλά επιστρέφεται 1, -1 αν κάτι έγινε λάθος.
#### `void parse_json_file(char *name,char* spec_id,HashTable **ht)` 
H συνάρτηση αναλύει το json file που άνοιξε σε keys - values. Ότι είναι αριστερά από το χαρακτήρα ':' είναι key. Και το key μπορεί να έχει κάποιους χαρακτήρες ("/ "") που δεν θέλουμε να λάβουμε υπ'όψιν. Έπειτα ελέγχουμε αν το value δεν έχει παραπάνω από ένα στοιχείο, αφαιρούμε τους περιττούς χαρακτήρες, και δημιουργούμε το dictionary που δημιουργήσαμε για το spce_id. Αλλιώς μετράμε τις γραμμές που μπορεί να έχουν τα values, μέχρι να φτάσουμε στο χαρατκήρα ']' δημιουργούμε έναν πίνακα με τα values και τα αποθηκεύουμε στο dictionary. Όλοι αυτή η διαδικασία επαναλαμβάνεται για κάθε γραμή key - value του .json αρχείου. Όταν τελειώσει η ανάγνωση του αρχείου τότε εισάγουμε το ολοκληρωμένο dictionary στο HashTable μας απελευθερώνουμε κατάλληλα τη δεσμευμένη μνήμη.
## main.c
Η main βρίσκει σε ποιο σημείο βρίσκεται το αρχείο των data. Δημιουργεί το HashTable που πρέπει να αποθηκεύσει τις κλίκες. Δημιουργεί το vocabulary. Περνάει για πρώτη φορά τις κλίκες από το dataset_X. Έπειτα διαβάζει το αρχείο του csv και δημιουργεί τις υπόλοιπες κλίκες. Καλεί τη συνάρτηση για να γίνει train ένα νέο μοντέλο logisticRegression και το αποθηκεύει σε κάποιο αρχείο. Τέλος γράφει τις κλίκες σε ένα άλλο αρχείο, και απελευθερώνει κατάλληλα τη μνήμη.
## Inference.c
Παρόμοια με τη main, δημιουργεί το HashTable, το vocabulary το διαβάζει από αρχείο όπως και το μοντέλο του Logistic Regression. Έπειτα προβλέπει τις τιμές του δεδομένου filename καλώντας τη `csvInference`. Απελευθερώνει τη μνήμη και τερματίζει.
# ErrorHandling
1. Αν δεν ανοίξει ο φάκελος, και έχει αποτύχει η opendir()
2. Αν δεν βρεθεί το αρχείο dataset_X
3. Aν δεν ανοίξει σωστά κάποιο JSON
4. Αν αποτύχει η malloc()
5. Αν αποτύχει να ανοίξει κάποιο αρχείο
6. Αν αποτύχει να γράψει σε κάποιο αρχείο
7. Aν κάποιο flag έχει δοθεί πολλές φορές
8. Αν έχει δοθεί λάθος argument στις main
# Χρήσιμες εντολές
### make - Για να δημιουργηθεί η main και το inference
### make test - Για να δημιουργηθούν τα test cases του accutest
### ./test - Για τα test files
### ./learning_test - Για τα test unit του preprocessing και του machine learning
### ./main -f filename -b bow-type -v vector-type -n negative-mode - Για να τρέξει η main
### e.g: ./main -f sigmod_large_labelled_dataset.csv -b tf-idf -v abs -n on 
### ./inferece -f filename - Για να τρέξει το inference, ποτέ πριν τη main, δηλαδή αν δεν υπάρχει ήδη κάποιο αρχείο stats.txt
### e.g : ./inference -f sigmod_large_labelled_dataset.csv
### make clean - Για να σβηστούν τα .o και εκτελέσιμα
### Παρατηρήσεις
1. Το αρχείο που είναι να γίνει το inference να είναι τύπου, *sigmod_large_labelled_dataset.csv*. Δλδ στη πρώτη γραμμή να περιέχεται το [left_spec,right_spec,label] και έπειτα τα δεδομένα να είναι αυτού του τύπου.


