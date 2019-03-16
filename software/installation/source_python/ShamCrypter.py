# -*- coding: utf-8 -*-
import hashlib
import csv

class ShamCrypter(object):
    def __init__(self):
        self.salt = "This is a string that only the person knows that does not conduct the study and has no influence on the progress of the experiment progression or the subjects and their direct exclusion"

    def encode(self, subjectName, conditionSubjectCodename, condition):
        return hashlib.sha512(str(subjectName) + str(conditionSubjectCodename) + str(condition) + self.salt).hexdigest()

    def testConditionIsSham(self, subjectName, conditionSubjectCodename, encryptedCondition):
        return hashlib.sha512(str(subjectName) + str(conditionSubjectCodename) + str("sham") + self.salt).hexdigest() == encryptedCondition

    def testConditionIsStim(self, subjectName, conditionSubjectCodename, encryptedCondition):
        return hashlib.sha512(str(subjectName) + str(conditionSubjectCodename) + str("stim") + self.salt).hexdigest() == encryptedCondition

    def testCondtionIsStimOrSham(self, subjectName, conditionSubjectCodename, encryptedCondition):
        return self.testConditionIsStim(subjectName, conditionSubjectCodename, encryptedCondition) or self.testConditionIsSham(subjectName, conditionSubjectCodename, encryptedCondition)

    def testConditionIsString(self, subjectName, conditionSubjectCodename, encryptedCondition, string):
        return hashlib.sha512(str(subjectName) + str(conditionSubjectCodename) + str(string) + self.salt).hexdigest() == encryptedCondition

    def readSubjectConditionListWriteEcrypted(self, subjectConditionListFilePathUncrypted, subjectConditionListFilePathEncrypted):

        with open(subjectConditionListFilePathUncrypted, 'rb', buffering=20000000) as uncryptedListFileHandle:
            with open(subjectConditionListFilePathEncrypted, 'wb', buffering=20000000) as encryptedListFileHandle:
                reader = csv.reader(uncryptedListFileHandle, delimiter=',')  # creates the reader object

                writer = csv.writer(encryptedListFileHandle, delimiter=',', quoting=csv.QUOTE_NONE)
                writer.writerow(['subject', 'condition_encoded', 'subject_condition_code'])

                for row in reader:  # iterates the rows of the file in orders
                    # print(row)  # prints each row
                    if reader.line_num == 1:
                        # subject,condition,subject_condition_code
                        continue  # skip the first header line
                    else:
                        last_line = row
                        subject = last_line[0]
                        condition = last_line[1]
                        subject_condition_code = last_line[2]
                        writer.writerow([subject, self.encode(subject,subject_condition_code,condition), subject_condition_code])

                encryptedListFileHandle.flush()

            encryptedListFileHandle.close()
        uncryptedListFileHandle.close()

        return True

    def isSubjectContainedInSubjectConditionListWriteEcrypted(self, subject_to_be_checked, subjectConditionListFilePathEncrypted):
        contained = False
        with open(subjectConditionListFilePathEncrypted, 'rb', buffering=20000000)  as encryptedListFileHandle:
            reader = csv.reader(encryptedListFileHandle, delimiter=',')  # creates the reader object

            for row in reader:  # iterates the rows of the file in orders
                # print(row)  # prints each row
                if reader.line_num == 1:
                    # subject,condition,subject_condition_code
                    continue  # skip the first header line
                else:
                    last_line = row
                    subject = last_line[0]
                    contained = contained or (subject == subject_to_be_checked)

            encryptedListFileHandle.close()
        return contained

    def isStimOrShamConditionBySubjectConditionListWriteEcrypted(self, subject_to_be_checked, subjectConditionListFilePathEncrypted):
        isStimOrSham = False
        stimOrSham = ""
        with open(subjectConditionListFilePathEncrypted, 'rb', buffering=20000000)  as encryptedListFileHandle:
            reader = csv.reader(encryptedListFileHandle, delimiter=',')  # creates the reader object

            for row in reader:  # iterates the rows of the file in orders
                # print(row)  # prints each row
                if reader.line_num == 1:
                    # subject,condition,subject_condition_code
                    continue  # skip the first header line
                else:
                    last_line = row
                    subject = last_line[0]
                    condition_encoded = last_line[1]
                    subject_condition_code = last_line[2]
                    if subject == subject_to_be_checked:
                        isStimOrSham = self.testCondtionIsStimOrSham(subject, subject_condition_code, condition_encoded)
                        if self.testConditionIsStim(subject, subject_condition_code, condition_encoded):
                            stimOrSham = "stim"
                        elif self.testConditionIsSham(subject, subject_condition_code, condition_encoded):
                            stimOrSham = "sham"
                        break
            encryptedListFileHandle.close()
        return (isStimOrSham, stimOrSham, subject_condition_code)
